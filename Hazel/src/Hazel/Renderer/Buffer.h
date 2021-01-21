#pragma once

namespace Hazel {

	//enables us to use switch on the different data that can be in a shader
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	//calculates the memory size of each type
	static uint32_t ShaderdataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:  return sizeof(float);
		case ShaderDataType::Float2: return sizeof(float) * 2;
		case ShaderDataType::Float3: return sizeof(float) * 3;
		case ShaderDataType::Float4: return sizeof(float) * 4;
		case ShaderDataType::Mat3:	 return sizeof(float) * 3 * 3;
		case ShaderDataType::Mat4:	 return sizeof(float) * 4 * 4;
		case ShaderDataType::Int:	 return sizeof(int);
		case ShaderDataType::Int2:	 return sizeof(int) * 2;
		case ShaderDataType::Int3:	 return sizeof(int) * 3;
		case ShaderDataType::Int4:	 return sizeof(int) * 4;
		case ShaderDataType::Bool:	 return sizeof(bool);
		}
		HZ_CORE_ASSERT(false, "Unknown type given")
		return 0;
	}

	//stores the name of the element, type, and other data calculated for you like size and offset
	struct BufferElements
	{
	public:
		std::string name;
		ShaderDataType type;
		uint32_t size;
		size_t offset;
		bool normalized;

		BufferElements() = default;

		BufferElements(ShaderDataType Type, const std::string Name, bool norm = false)
			:name(Name), type(Type), size(ShaderdataTypeSize(type)), offset(0), normalized(norm) {}

		uint32_t GetComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Mat3:    return 3; // 3* float3
			case ShaderDataType::Mat4:    return 4; // 4* float4
			case ShaderDataType::Int:     return 1;
			case ShaderDataType::Int2:    return 2;
			case ShaderDataType::Int3:    return 3;
			case ShaderDataType::Int4:    return 4;
			case ShaderDataType::Bool:    return 1;
			}
			HZ_CORE_ASSERT(false, "unknown shaderdatatype")
			return 0;
		}
	};

	//a wrapper class for a std::vector of buffer elements, order of vector matters
	class BufferLayout
	{
	public:
		BufferLayout(const std::initializer_list<BufferElements>& Element)
			:m_Element(Element)
		{
			CalculateOffsetAndStride();
		}
		BufferLayout() {}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElements>& GetElements() const { return m_Element; }

		std::vector<BufferElements>::iterator begin() { return m_Element.begin(); }
		std::vector<BufferElements>::iterator end() { return m_Element.end(); }
		std::vector<BufferElements>::const_iterator begin() const { return m_Element.begin(); }
		std::vector<BufferElements>::const_iterator end() const { return m_Element.end(); }
	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& Element : m_Element) {
				Element.offset = offset;
				offset += Element.size;
				m_Stride += Element.size;
			}
		}
	private:
		std::vector<BufferElements> m_Element;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
		static Ref<VertexBuffer> Create(uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t size);
	};

}