#pragma once
#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {

	class OpenGLVertexArray : public VertexArray
	{

	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vb) override;
		virtual void AddIndexBuffer(const Ref<IndexBuffer>& ib) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override {return m_VertexBuffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffers; }
		private:
		uint32_t m_RendererId;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffers;
	};

}