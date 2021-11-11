#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "Hazel/Renderer/UniformBuffer.h"
#include "RenderCommand.h"

#include "glm/gtc/type_ptr.hpp"

namespace Hazel {

	//struct containing core properties of a rectangle
	struct QuadVertex
	{
		glm::vec3 pos;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
		float TilingFactor;
		int   ID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor-only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor-only
		int EntityID;
	};



	//data for the renderer
	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 4000;
		static const uint32_t MaxVerticies = MaxQuads * 4;
		static const uint32_t Maxindicies = MaxQuads * 6;
		static const uint32_t MaxTexture = 32;

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> quadShader;

		Ref<VertexArray> circleVertexArray;
		Ref<VertexBuffer> circleVertexBuffer;
		Ref<Shader> circleShader;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;


		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		uint32_t circleIndexCount = 0;
		CircleVertex* circleVertexBufferBase = nullptr;
		CircleVertex* circleVertexBufferPtr = nullptr;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		float LineWidth = 1.0f;


		std::array<Ref<Texture2D>, MaxTexture> textureSlots;
		uint32_t TextureSlotIndex = 1; //0 = whiteTexture

		glm::vec4 quadVertexPositions[4] = { glm::vec4(0) };

		Renderer2D::Statistics stats;


		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData s_Data;

	//initilizes the renderer
	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();

		s_Data.quadVertexArray = (VertexArray::Create());

		s_Data.quadVertexBuffer = VertexBuffer::Create(s_Data.MaxVerticies * sizeof(QuadVertex));
		//data is given to gpu in this order, make sure shader matches this order
		s_Data.quadVertexBuffer->SetLayout({
				{ShaderDataType::Float3, "position"},
				{ShaderDataType::Float4, "color"},
				{ShaderDataType::Float2, "texCoord"},
				{ShaderDataType::Float , "texIndex"},
				{ShaderDataType::Float , "tilingFactor"},
				{ShaderDataType::Int   , "EntityID"}
			});
		s_Data.quadVertexArray->AddVertexBuffer(s_Data.quadVertexBuffer);

		s_Data.quadVertexBufferBase = new QuadVertex[s_Data.MaxVerticies];
		uint32_t* quadIndices = new uint32_t[s_Data.Maxindicies];

		uint32_t offset = 0;
		for (int i = 0; i < s_Data.Maxindicies; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadib = IndexBuffer::Create(quadIndices, s_Data.Maxindicies);
		s_Data.quadVertexArray->AddIndexBuffer(quadib);


		s_Data.circleVertexArray = VertexArray::Create();
		s_Data.circleVertexBuffer = VertexBuffer::Create(s_Data.MaxVerticies * sizeof(CircleVertex));

		s_Data.circleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade"          },
			{ ShaderDataType::Int,    "a_EntityID"      }
			});
		s_Data.circleVertexArray->AddVertexBuffer(s_Data.circleVertexBuffer);
		s_Data.circleVertexBufferBase = new CircleVertex[s_Data.MaxVerticies];
		s_Data.circleVertexArray->AddIndexBuffer(quadib);


		// Lines
		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVerticies * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVerticies];


		delete[] quadIndices;

		//makes a blank white texture

		int32_t samplers[s_Data.MaxTexture];
		for (uint32_t i = 0; i < s_Data.MaxTexture; i++)
			samplers[i] = i;

		//loads the shader to be used
		s_Data.quadShader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");
		s_Data.circleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");
		s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Lines.glsl");

		s_Data.textureSlots[0] = Texture2D::Create();


		s_Data.quadVertexPositions[0] = { -.5, -.5, 0, 1 };
		s_Data.quadVertexPositions[1] = { .5, -.5, 0, 1 };
		s_Data.quadVertexPositions[2] = { .5,  .5, 0, 1 };
		s_Data.quadVertexPositions[3] = { -.5,  .5, 0, 1 };

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();

		delete[] s_Data.quadVertexBufferBase;
	}

	static void StartNewBatch()
	{
		Renderer2D::EndScene();

		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.circleIndexCount = 0;
		s_Data.circleVertexBufferPtr = s_Data.circleVertexBufferBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;


		s_Data.TextureSlotIndex = 1;
	}


	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.circleIndexCount = 0;
		s_Data.circleVertexBufferPtr = s_Data.circleVertexBufferBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}
	void Renderer2D::BeginScene(const OrthographicCamera& cam)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = cam.GetViewProjectionMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));
		
		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.circleIndexCount = 0;
		s_Data.circleVertexBufferPtr = s_Data.circleVertexBufferBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}
	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));
		
		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.circleIndexCount = 0;
		s_Data.circleVertexBufferPtr = s_Data.circleVertexBufferBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}
	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();

		Flush();
	}
	void Renderer2D::Flush()
	{
		if (s_Data.quadIndexCount)
		{

			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.quadVertexBufferPtr - (uint8_t*)s_Data.quadVertexBufferBase);
			s_Data.quadVertexBuffer->SetData(s_Data.quadVertexBufferBase, dataSize);

			//bind textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.textureSlots[i]->Bind(i);

			s_Data.quadShader->Bind();
			RenderCommand::DrawIndexed(s_Data.quadVertexArray, s_Data.quadIndexCount);

			s_Data.stats.drawCalls++;
		}
		if (s_Data.circleIndexCount)
		{

			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.circleVertexBufferPtr - (uint8_t*)s_Data.circleVertexBufferBase);
			s_Data.circleVertexBuffer->SetData(s_Data.circleVertexBufferBase, dataSize);

			s_Data.circleShader->Bind();
			RenderCommand::DrawIndexed(s_Data.circleVertexArray, s_Data.circleIndexCount);

			s_Data.stats.drawCalls++;
		}
		if (s_Data.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

			s_Data.LineShader->Bind();
			RenderCommand::SetLineWidth(s_Data.LineWidth);
			RenderCommand::DrawIndexed(s_Data.LineVertexArray, s_Data.LineVertexCount, RendererAPI::RenderType::LINES);
			s_Data.stats.drawCalls++;
		}
	}


	void Renderer2D::DrawQuad(const uint32_t ID, const glm::vec3& pos, const glm::vec2& size, const float& radianAngle, Ref<Texture2D>& tex, float tilingFactor, const Color& color)
	{

		HZ_PROFILE_FUNCTION();

		glm::mat4 transform;
		if (radianAngle == 0)
		{
			transform = glm::translate(glm::mat4(1), pos) *
				glm::scale(glm::mat4(1), { size.x, size.y, 1 });
		}
		else
		{
			transform = glm::translate(glm::mat4(1), pos) *
				glm::rotate(glm::mat4(1), radianAngle, { 0, 0, 1 }) *
				glm::scale(glm::mat4(1), { size.x, size.y, 1 });
		}

		DrawQuad(ID, transform, tex, tilingFactor, color);

	}

	void Renderer2D::DrawQuad(uint32_t ID, const glm::mat4& transform, Ref<Texture2D>& tex, float tilingFactor, const Color& color)
	{


		HZ_PROFILE_FUNCTION();

		if (!tex)
		{
			tex = s_Data.textureSlots[0];
		}

		if (s_Data.quadIndexCount >= s_Data.Maxindicies)
		{
			StartNewBatch();
		}

		float textureIndex = 0;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i].get() == *tex.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.textureSlots[s_Data.TextureSlotIndex++] = tex;
		}

		

		for (int i = 0; i < 4; i++)
		{
			s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[i];
			s_Data.quadVertexBufferPtr->color = color.GetVec4();
			s_Data.quadVertexBufferPtr->texCoord = tex->GetTextureCoordinates()[i];
			s_Data.quadVertexBufferPtr->texIndex = textureIndex;
			s_Data.quadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.quadVertexBufferPtr->ID = (int)ID;
			s_Data.quadVertexBufferPtr++;
		}

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/, int entityID /*= -1*/)
	{
		HZ_PROFILE_FUNCTION();

		// TODO: implement for circles
		// if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		// 	NextBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.circleVertexBufferPtr->WorldPosition = transform * s_Data.quadVertexPositions[i];
			s_Data.circleVertexBufferPtr->LocalPosition = s_Data.quadVertexPositions[i] * 2.0f;
			s_Data.circleVertexBufferPtr->Color = color;
			s_Data.circleVertexBufferPtr->Thickness = thickness;
			s_Data.circleVertexBufferPtr->Fade = fade;
			s_Data.circleVertexBufferPtr->EntityID = entityID;
			s_Data.circleVertexBufferPtr++;
		}

		s_Data.circleIndexCount += 6;

		s_Data.stats.quadCount++;
	}


	void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const Color& color, int entityID)
	{
		s_Data.LineVertexBufferPtr->Position = p0;
		s_Data.LineVertexBufferPtr->Color = color.GetVec4();
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = p1;
		s_Data.LineVertexBufferPtr->Color = color.GetVec4();
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const Color& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const Color& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Data.quadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	}



	void Renderer2D::DrawSprite(const glm::mat4& transform, Component::SpriteRenderer& src, int entityID)
	{
		if (src.Tex)
			DrawQuad(entityID, transform, src.color, src.Tex, src.TilingFactor);
		else
			DrawQuad(entityID, transform, src.color);
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Data.LineWidth = width;
	}


	void Renderer2D::ResetStats() { s_Data.stats = { 0, 0, Renderer2DData::MaxQuads }; }

	Renderer2D::Statistics& Renderer2D::GetStats() { return s_Data.stats; }
}