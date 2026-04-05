#include "Renderer/VertexArray.hpp"
#include "Platforms/OpenGL/OpenGLVertexArray.hpp"

namespace NFSEngine {
	VertexArray* VertexArray::Create() {
		return new OpenGLVertexArray();
	}
}