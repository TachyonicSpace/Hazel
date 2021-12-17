#include "hzpch.h"
#include "OpenGLRendererAPI.h"
#include "Hazel/Core/Application.h"

#include "glad/glad.h"

namespace Hazel {

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         HZ_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       HZ_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_LOW:          HZ_CORE_TRACE(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: HZ_CORE_INFO(message); return;
		}

		HZ_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init(bool blend)
	{
		HZ_PROFILE_FUNCTION();

#ifdef HZ_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif




		if (blend) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		srcFactor = Blending::Types::SRC_ALPHA;
		dstFactor = Blending::Types::ONE_MINUS_SRC_ALPHA;
		blendFunc = Blending::Functions::FUNC_ADD;

		//glEnable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
		//glEnable(GL_LINE_STIPPLE);
		//glEnable(GL_POINT_SMOOTH);
		//glEnable(GL_POLYGON_SMOOTH);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		if (!(Application::Get().m_Minimized))
			glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCount, RenderType Primitives /*= RenderType::GL_TRIANGLES*/)
	{
		va->Bind();
		auto count = indexCount ? indexCount : va->GetIndexBuffer()->GetCount();

		/*
		GL_POINTS
		GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP
		GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN
		*/

		if (!(Application::Get().m_Minimized))
		{
			if (Primitives == RenderType::LINES || Primitives == RenderType::LINE_STRIP || Primitives == RenderType::LINE_LOOP)
				glDrawArrays((GLenum)Primitives, 0, indexCount);
			else
				glDrawElements((GLenum)Primitives, count, GL_UNSIGNED_INT, nullptr);
		}
	}
	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

	int OpenGLRendererAPI::convertTypesToOpenGLEnum(Blending::Types bt)
	{
		switch (bt)
		{
		case Hazel::Blending::Types::ZERO:
			return GL_ZERO;
			break;
		case Hazel::Blending::Types::ONE:
			return GL_ONE;
			break;
		case Hazel::Blending::Types::SRC_COLOR:
			return GL_SRC_COLOR;
			break;
		case Hazel::Blending::Types::ONE_MINUS_SRC_COLOR:
			return GL_ONE_MINUS_SRC_COLOR;
			break;
		case Hazel::Blending::Types::DST_COLOR:
			return GL_DST_COLOR;
			break;
		case Hazel::Blending::Types::ONE_MINUS_DEST_COLOR:
			return GL_ONE_MINUS_DST_COLOR;
			break;
		case Hazel::Blending::Types::SRC_ALPHA:
			return GL_SRC_ALPHA;
			break;
		case Hazel::Blending::Types::ONE_MINUS_SRC_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
			break;
		case Hazel::Blending::Types::DST_ALPHA:
			return GL_DST_ALPHA;
			break;
		case Hazel::Blending::Types::ONE_MINUS_DST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
			break;
		case Hazel::Blending::Types::CONSTANT_COLOR:
			return GL_CONSTANT_COLOR;
			break;
		case Hazel::Blending::Types::ONE_MINUS_CONSTANT_COLOR:
			return GL_ONE_MINUS_CONSTANT_COLOR;
			break;
		case Hazel::Blending::Types::CONSTANT_ALPHA:
			return GL_CONSTANT_ALPHA;
			break;
		case Hazel::Blending::Types::ONE_MINUS_CONSTANT_ALPHA:
			return GL_ONE_MINUS_CONSTANT_ALPHA;
			break;
		case Hazel::Blending::Types::SRC1_ALPHA:
			return GL_SRC1_ALPHA;
			break;
		case Hazel::Blending::Types::ONE_MINUS_SRC1_ALPHA:
			return GL_ONE_MINUS_SRC1_ALPHA;
			break;
		case Hazel::Blending::Types::SRC1_COLOR:
			return GL_SRC1_COLOR;
			break;
		case Hazel::Blending::Types::ONE_MINUS_SRC1_COLOR:
			return GL_ONE_MINUS_SRC1_COLOR;
			break;
		default:
			return GL_ONE;
			break;
		}
	}

	int OpenGLRendererAPI::convertEquationToOpenGLEnum(Blending::Functions bf)
	{
		switch (bf)
		{
		case Hazel::Blending::Functions::FUNC_ADD:
			return GL_FUNC_ADD;
		case Hazel::Blending::Functions::FUNC_SUBTRACT:
			return GL_FUNC_SUBTRACT;
		case Hazel::Blending::Functions::FUNC_REVERSE_SUBTRACT:
			return GL_FUNC_REVERSE_SUBTRACT;
		case Hazel::Blending::Functions::MIN:
			return GL_MIN;
		case Hazel::Blending::Functions::MAX:
			return GL_MAX;
		default:
			return GL_FUNC_ADD;
			break;
		}
	}

	void OpenGLRendererAPI::SetViewport(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax)
	{
		glViewport(xMin, yMin, xMax, yMax);
	}


	void OpenGLRendererAPI::SetSrcFactor(const Blending::Types& bt) 
	{
		srcFactor = bt; 
		glBlendFunc(convertTypesToOpenGLEnum(srcFactor), convertTypesToOpenGLEnum(dstFactor));
	};
	void OpenGLRendererAPI::SetDstFactor(const Blending::Types& bt) 
	{
		dstFactor = bt; 
		glBlendFunc(convertTypesToOpenGLEnum(srcFactor), convertTypesToOpenGLEnum(dstFactor));
	}
	void OpenGLRendererAPI::SetBlendFunc(const Blending::Functions& bf) 
	{
		blendFunc = bf; 
		glBlendEquation(convertEquationToOpenGLEnum(bf));
	}

	void tmp()
	{
		//glEnable
		{
			glEnable(GL_ALPHA_TEST);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_LINE_STIPPLE);
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_POLYGON_SMOOTH);
		}

		//	resultColor = blendEquation(	src*sfactor,	dst*dfactor	)
		//glBlendFunc
		/*
					i | buff,	the index of the draw buffer to blend
					range( 0, gl_max_draw_buffers)

					sfactor,						what to multiply the src colors with
					dfactor,						what to multiply the destination colors with

					GL_ZERO,						sets to 0(black)
					GL_ONE,							sets to 1(one)
					GL_SRC_COLOR					use src color
					GL_ONE_MINUS_SRC_COLOR			subtract all the src channels from one
					GL_DST_COLOR					multiplies all values by dst color
					GL_ONE_MINUS_DEST_COLOR			subtract all the dst channels from one
					GL_SRC_ALPHA					multiplies all values by src alpha
					GL_ONE_MINUS_SRC_ALPHA			1-GL_SRC_ALPHA
					GL_DST_ALPHA					multiplies all values by dst alpha
					GL_ONE_MINUS_DST_ALPHA			1-GL_DST_ALPHA
					GL_ALPHA_SATURATE				sets alpha to one

					GL_CONSTANT_COLOR				set to const color
					GL_ONE_MINUS_CONSTANT_COLOR		1-GL_CONSTANT_COLOR
					GL_CONSTANT_ALPHA				sets alpha to constant value
					GL_ONE_MINUS_CONSTANT_ALPHA		1-GL_CONSTANT_ALPHA
					GL_SRC1_ALPHA					multiplies all values by src1 alpha
					GL_ONE_MINUS_SRC1_ALPHA			1-GL_SRC1_ALPHA
					GL_SRC1_COLOR					use src1 color
					GL_ONE_MINUS_SRC1_COLOR			subtract all the src1 channels from one
		*/
		//glBlendEquation
		/*
				GL_FUNC_ADD					src+dst
				GL_FUNC_SUBTRACT			src-dst
				GL_FUNC_REVERSE_SUBTRACT	dst-src
				GL_MIN						min(src, dst)
				GL_MAX						max(src, dst)
		
		
		*/
	}
}