#pragma once
#include <vector>
#include "GL/glew.h"

struct VertexBufferElement
{
	unsigned int count;
	unsigned int type;
	bool normalized;
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0) {}

	template <typename T>
	void Push(int count)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, false });
		m_Stride += sizeof(GLfloat);
	}

	template<>
	void Push<unsigned int>(int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, false });
		m_Stride += sizeof(GLuint);
	}

	template<>
	void Push<unsigned char>(int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, false });
		m_Stride += sizeof(GLbyte);
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements;  }
	inline unsigned int GetStride() const { return m_Stride; }

};

