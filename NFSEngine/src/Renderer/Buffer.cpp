#include "Buffer.hpp"

#include "Platforms/OpenGL/OpenGLBuffer.hpp"

namespace NFSEngine {

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size) 
	{
		// this should use some RendererApi to determine which Api to use 
		return new OpenGLVertexBuffer(vertices, size);
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		// this should use some RendererApi to determine which Api to use 
		return new OpenGLIndexBuffer(indices, size);
	}

}