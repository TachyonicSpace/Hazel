#include "hzpch.h"
#include "OpenGLVertexArray.h"

#include "glad/glad.h"

namespace Hazel {

	static GLenum ShaderdataTypeToGLEnum(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:  return GL_FLOAT;
			case ShaderDataType::Float2: return GL_FLOAT;
			case ShaderDataType::Float3: return GL_FLOAT;
			case ShaderDataType::Float4: return GL_FLOAT;
			case ShaderDataType::Mat3:	 return GL_FLOAT;
			case ShaderDataType::Mat4:	 return GL_FLOAT;
			case ShaderDataType::Int:	 return GL_INT;
			case ShaderDataType::Int2:	 return GL_INT;
			case ShaderDataType::Int3:	 return GL_INT;
			case ShaderDataType::Int4:	 return GL_INT;
			case ShaderDataType::Bool:	 return GL_BOOL;
		}
		HZ_CORE_ASSERT(false, "Unknown type given")
			return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}


	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}
	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vb)
	{
		HZ_CORE_ASSERT(vb->GetLayout().GetElements().size(), "Vertex buffer has no layout");

		glBindVertexArray(m_RendererID);
		vb->Bind();

		int i = 0;
		const auto& layout = vb->GetLayout();
		for (const auto& Element : layout) {
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i,
				Element.GetComponentCount(),
				ShaderdataTypeToGLEnum(Element.type),
				Element.normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)Element.offset);
			i++;
		}
		m_VertexBuffers.push_back(vb);
	}
	void OpenGLVertexArray::AddIndexBuffer(const Ref<IndexBuffer>& ib)
	{
		glBindVertexArray(m_RendererID);
		ib->Bind();

		m_IndexBuffers = ib;
	}

}