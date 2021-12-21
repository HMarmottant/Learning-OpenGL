#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <time.h>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Window.h"

#pragma region glDebugMessageCallback
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute
// this software, either in source code form or as a compiled binary, for any
// purpose, commercial or non-commercial, and by any means.
// 
// In jurisdictions that recognize copyright laws, the author or authors of this
// software dedicate any and all copyright interest in the software to the
// public domain. We make this dedication for the benefit of the public at large
// and to the detriment of our heirs and successors. We intend this dedication
// to be an overt act of relinquishment in perpetuity of all present and future
// rights to this software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <http://unlicense.org/>

// REQUIREMENTS: OpenGL version with the KHR_debug extension available.

// Callback function for printing debug statements
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* msg, const void* data)
{
	std::string _source;
	std::string _type;
	std::string _severity;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		_source = "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_source = "WINDOW SYSTEM";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_source = "SHADER COMPILER";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_source = "THIRD PARTY";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		_source = "APPLICATION";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		_source = "UNKNOWN";
		break;

	default:
		_source = "UNKNOWN";
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		_type = "ERROR";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_type = "DEPRECATED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_type = "UDEFINED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		_type = "PORTABILITY";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		_type = "PERFORMANCE";
		break;

	case GL_DEBUG_TYPE_OTHER:
		_type = "OTHER";
		break;

	case GL_DEBUG_TYPE_MARKER:
		_type = "MARKER";
		break;

	default:
		_type = "UNKNOWN";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		_severity = "HIGH";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		_severity = "MEDIUM";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		_severity = "LOW";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		_severity = "NOTIFICATION";
		break;

	default:
		_severity = "UNKNOWN";
		break;
	}

	std::cout << id << " : " << _type << " of " << _severity << " severity, raised from " << _source << " : " << msg << std::endl;
	if (_severity == "HIGH") __debugbreak();
}
#pragma endregion

double scroll = 1;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(yoffset > 0 || scroll > 1) scroll += yoffset;
}


int main(void)
{
	Window window = Window::Window("Mandelbrot",640,480,true);
	glfwSetScrollCallback(window.m_Window, scroll_callback);
	std::cout << glGetString(GL_VERSION) << std::endl;
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nb of vertex attributes supported: " << nrAttributes << std::endl;

	{
#pragma region INIT DEBUG OUTPUT
		// =============== INIT DEBUG OUTPUT ================
		// The following function calls should be made directly after OpenGL
		// initialization.

		// Enable the debugging layer of OpenGL
		//
		// GL_DEBUG_OUTPUT - Faster version but not useful for breakpoints
		// GL_DEBUG_OUTPUT_SYNCHRONUS - Callback is in sync with errors, so a breakpoint
		// can be placed on the callback in order to get a stacktrace for the GL error.

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		// Set the function that will be triggered by the callback, the second parameter
		// is the data parameter of the callback, it can be useful for different
		// contexts but isn't necessary for our simple use case.
		glDebugMessageCallback(GLDebugMessageCallback, NULL);
#pragma endregion


		float positions[] = {
			-1.0f, -1.0f,
			 1.0f, -1.0f,
			 1.0f,  1.0f,
			-1.0f,  1.0f
		};

		unsigned int indices[] = {
			0,1,2,
			2,3,0
		};

		VertexArray va;

		VertexBuffer vb(positions, 4 * 2 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		Shader shader("./res/shaders/mandelbrot.shader");
		shader.Bind();

		float u_Offset[] = { -2.0f, -1.5f };
		float u_FragDelta = 2.0f / 480.0f;
		float u_Iterations = 50.0f;

		shader.SetUniform2f("u_Offset", u_Offset);
		shader.SetUniform1f("u_FragDelta", u_FragDelta);
		shader.SetUniform1f("u_Iterations", u_Iterations);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;


		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window.m_Window))
		{

			

			/* Render here */
			renderer.Clear();

			shader.Bind();

			shader.SetUniform2f("u_Offset", u_Offset);
			shader.SetUniform1f("u_FragDelta", u_FragDelta);
			shader.SetUniform1f("u_Iterations", u_Iterations);

			/*va.Bind();
			vb.Bind();
			ib.Bind();*/

			renderer.Draw(va, ib, shader);
			

			window.Update();

			u_FragDelta = (2.0f / 480.0f) * pow(0.9,scroll); 
			std::cout << u_FragDelta << std::endl;

			double screenDrag[2] = {0,0};
			window.GetMouseScreenDrag(screenDrag);
			u_Offset[0] += screenDrag[0] * u_FragDelta;
			u_Offset[1] += screenDrag[1] * u_FragDelta;

			
		}
	}
	glfwTerminate();
	return 0;
}

