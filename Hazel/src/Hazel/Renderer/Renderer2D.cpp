#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

namespace Hazel {

	struct QuadVertex
	{
		glm::vec3 pos;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
		float TilingFactor;
		int ObjectID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 150;
		static const uint32_t MaxVerticies = MaxQuads * 4;
		static const uint32_t Maxindicies = MaxQuads * 6;
		static const uint32_t MaxTexture = 32;

		Ref<VertexArray> va;
		Ref<VertexBuffer> vb;
		Ref<Shader> texShader;
		Ref<Texture2D> whiteTexture;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTexture> textureSlots;
		uint32_t TextureSlotIndex = 1; //0 = whiteTexture

		glm::vec4 quadVertexPositions[4];

		Renderer2D::Statistics stats;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();

		s_Data.va = (VertexArray::Create());

		s_Data.vb = VertexBuffer::Create(s_Data.MaxVerticies * sizeof(QuadVertex));
		s_Data.vb->SetLayout({
				{ShaderDataType::Float3, "position"},
				{ShaderDataType::Float4, "color"},
				{ShaderDataType::Float2, "texCoord"},
				{ShaderDataType::Float , "texIndex"},
				{ShaderDataType::Float , "tilingFactor"},
				{ShaderDataType::Int   , "objectID"}
			});
		s_Data.va->AddVertexBuffer(s_Data.vb);

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
		s_Data.va->AddIndexBuffer(quadib);

		delete[] quadIndices;


		s_Data.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.whiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

		int32_t samplers[s_Data.MaxTexture];
		for (uint32_t i = 0; i < s_Data.MaxTexture; i++)
			samplers[i] = i;

		s_Data.texShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.texShader->Bind();
		s_Data.texShader->UploadUniformIntArray("u_Texture", samplers, s_Data.MaxTexture);

		s_Data.textureSlots[0] = s_Data.whiteTexture;


		s_Data.quadVertexPositions[0] = { -.5, -.5, 0, 1 };
		s_Data.quadVertexPositions[1] = { .5, -.5, 0, 1 };
		s_Data.quadVertexPositions[2] = { .5,  .5, 0, 1 };
		s_Data.quadVertexPositions[3] = { -.5,  .5, 0, 1 };
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

		s_Data.TextureSlotIndex = 1;
	}


	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		HZ_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

		s_Data.texShader->Bind();
		s_Data.texShader->UploadUniformMat4("u_ViewProjection", viewProj);
		s_Data.stats.drawCalls = 0;
		s_Data.stats.quadCount = 0;
		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}
	void Renderer2D::BeginScene(const OrthographicCamera& cam)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.texShader->Bind();
		s_Data.texShader->UploadUniformMat4("u_ViewProjection", cam.GetViewProjectionMatrix());
		s_Data.stats.drawCalls = 0;
		s_Data.stats.quadCount = 0;
		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}
	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.GetViewProjection();
		s_Data.texShader->UploadUniformMat4("u_ViewProjection", viewProj);
		s_Data.stats.drawCalls = 0;
		s_Data.stats.quadCount = 0;
		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}
	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();

		Flush();
	}
	void Renderer2D::Flush()
	{
		if (s_Data.quadIndexCount == 0)
			return; // Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.quadVertexBufferPtr - (uint8_t*)s_Data.quadVertexBufferBase);
		s_Data.vb->SetData(s_Data.quadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.textureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_Data.va, s_Data.quadIndexCount);

		s_Data.stats.drawCalls++;
	}


	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Color& color, const Ref<Texture2D>& tex, float tilingFactor)
	{
		DrawQuad({ pos.x, pos.y, 0 }, size, color, tex, tilingFactor);
	}
	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Color& color, const Ref<Texture2D>& tex, float tilingFactor)
	{
		DrawQuad(pos, size, (Ref<Texture2D>)tex, tilingFactor, color);
	}
	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D>& tex, float tilingFactor, const Color& color)
	{
		DrawQuad({ pos.x, pos.y, 0 }, size, tex, tilingFactor, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D>& tex, float tilingFactor, const Color& color)
	{
		DrawQuad(pos, size, 0, tex, tilingFactor, color);
	}
	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const float& radianAngle, const Color& color, const Ref<Texture2D>& tex, float tilingFactor)
	{
		DrawQuad({ pos.x, pos.y, 0 }, size, radianAngle, color, tex, tilingFactor);
	}
	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const float& radianAngle, const Color& color, const Ref<Texture2D>& tex, float tilingFactor)
	{
		DrawQuad(pos, size, radianAngle, (Ref<Texture2D>)tex, tilingFactor, color);
	}
	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const float& radianAngle, Ref<Texture2D>& tex, float tilingFactor, const Color& color)
	{
		DrawQuad({ pos.x, pos.y, 0 }, size, radianAngle, tex, tilingFactor, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const float& radianAngle, Ref<Texture2D>& tex, float tilingFactor, const Color& color)
	{

		HZ_PROFILE_FUNCTION();

		/*if (!tex)
		{
			tex = s_Data.textureSlots[0];
		}

		if (s_Data.quadIndexCount >= s_Data.MaxQuads * 6)
			StartNewBatch();

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
		}*/

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

		DrawQuad(transform, tex, tilingFactor, color);
		/*for (int i = 0; i < 4; i++)
		{
			s_Data.quadVertexBufferPtr->pos = transform * s_Data.quadVertexPositions[i];
			s_Data.quadVertexBufferPtr->color = color.GetVec4();
			s_Data.quadVertexBufferPtr->texCoord = tex->GetTextureCoordinates()[i];
			s_Data.quadVertexBufferPtr->texIndex = textureIndex;
			s_Data.quadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.quadVertexBufferPtr++;
		}

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;*/
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Color& color, int entityID, const Ref<Texture2D>& tex, float tilingFactor)
	{
		DrawQuad(transform, (Ref<Texture2D>)tex, tilingFactor, color, entityID);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Color& color, const Ref<Texture2D>& tex /*= nullptr*/, float tilingFactor /*= 1.f*/, int entityID /*= 0*/)
	{
		DrawQuad(transform, (Ref<Texture2D>)tex, tilingFactor, color, entityID);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, Ref<Texture2D>& tex, float tilingFactor, const Color& color, int entityID)
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
			s_Data.quadVertexBufferPtr->ObjectID = entityID;
			s_Data.quadVertexBufferPtr++;
		}

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;
	}

	void Renderer2D::ResetStats()
	{
		s_Data.stats = { 0, 0 };
	}

	Renderer2D::Statistics& Renderer2D::GetStats() { return s_Data.stats; }
}