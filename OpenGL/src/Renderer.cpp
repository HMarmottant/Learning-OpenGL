#include "Renderer.h"

#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall()
{
	bool temp = true;
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] " << error << std::endl;
		temp = false;
	}
	return temp;
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();

	glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT);
}