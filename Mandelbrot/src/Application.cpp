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

#include "vendor/ArbitraryPrecisionFloat/ArbitraryPrecisionFloat.h"

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

int wWidth = 640;
int wHeight = 480;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scroll += yoffset;
}

void window_size_callback(GLFWwindow* window, int nwidth, int nheight)
{
	wWidth = nwidth;
	wHeight = nheight;
}

int main(void)
{
	Window window = Window::Window("Mandelbrot", 640, 480, true);
	glfwSetScrollCallback(window.m_Window, scroll_callback);
	glfwSetWindowSizeCallback(window.m_Window, window_size_callback);
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

		Shader shader("./res/shaders/basic.shader");
		shader.Bind();

		apfloat::apfloat u_Offsetx = apfloat::apfloat(std::to_string(-2.0f), 8, BASE10APF);
		apfloat::apfloat u_Offsety = apfloat::apfloat(std::to_string(-1.5f),8,BASE10APF) ;
		apfloat::apfloat u_FragDelta = apfloat::apfloat(std::to_string(2.0f / 480.0f),8,BASE10APF);
		float u_Iterations = 50.0f;

		extendedFloatArray offx = u_Offsetx.getasuintarray();
		extendedFloatArray offy = u_Offsety.getasuintarray();
		extendedFloatArray fragD = u_FragDelta.getasuintarray();

		shader.SetUniform1ui("u_Offsetx.sign", offx.sign);
		shader.SetUniform1uiv("u_Offsetx.num", offx.size,offx.array);

		shader.SetUniform1ui("u_Offsety.sign", offy.sign);
		shader.SetUniform1uiv("u_Offsety.num", offy.size, offy.array);

		shader.SetUniform1ui("u_FragDelta.sign", fragD.sign);
		shader.SetUniform1uiv("u_FragDelta.num", fragD.size, fragD.array);

		shader.SetUniform1f("u_Iterations", u_Iterations);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		double xpos, ypos;
		glfwGetCursorPos(window.m_Window, &xpos, &ypos);

		double previousScroll = scroll;
		apfloat::apfloat previousFragDelta = u_FragDelta;
		int previouswHeight = wHeight;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window.m_Window))
		{

			/* Render here */
			renderer.Clear();

			shader.Bind();

			extendedFloatArray offx = u_Offsetx.getasuintarray();
			extendedFloatArray offy = u_Offsety.getasuintarray();
			extendedFloatArray fragD = u_FragDelta.getasuintarray();

			shader.SetUniform1ui("u_Offsetx.sign", offx.sign);
			shader.SetUniform1uiv("u_Offsetx.num", offx.size, offx.array);

			shader.SetUniform1ui("u_Offsety.sign", offy.sign);
			shader.SetUniform1uiv("u_Offsety.num", offy.size, offy.array);

			shader.SetUniform1ui("u_FragDelta.sign", fragD.sign);
			shader.SetUniform1uiv("u_FragDelta.num", fragD.size, fragD.array);

			renderer.Draw(va, ib, shader);

			window.Update();

			u_Offsety = u_Offsety - (apfloat::apfloat(std::to_string(wHeight - previouswHeight),2,BASE10APF) * u_FragDelta);

			//std::cout << wHeight << std::endl;

			glfwGetCursorPos(window.m_Window, &xpos, &ypos);

			ypos = wHeight - ypos;

			//u_FragDelta = (2.0f / 480.0f) * pow(0.9, scroll);

			apfloat::apfloat apxpos = apfloat::apfloat(std::to_string(xpos), 2, BASE10APF);
			apfloat::apfloat apypos = apfloat::apfloat(std::to_string(ypos), 2, BASE10APF);

			apfloat::apfloat pt_fixe_x = u_Offsetx + (apxpos * previousFragDelta);
			apfloat::apfloat pt_fixe_y = u_Offsety + (apypos * previousFragDelta);

			/*std::cout << pt_fixe_x << std::endl;
			std::cout << pt_fixe_y << "\n" << std::endl;*/

			u_Offsetx = pt_fixe_x - (apxpos * u_FragDelta);
			u_Offsety = pt_fixe_y - (apypos * u_FragDelta);

			previousScroll = scroll;
			previousFragDelta = u_FragDelta;
			previouswHeight = wHeight;

			double screenDrag[2] = { 0,0 };
			window.GetMouseScreenDrag(screenDrag);
			u_Offsetx = u_Offsetx + (apfloat::apfloat(std::to_string(screenDrag[0]), 2, BASE10APF) * u_FragDelta);
			u_Offsety = u_Offsetx + (apfloat::apfloat(std::to_string(screenDrag[1]), 2, BASE10APF) * u_FragDelta);

		}
	}
	glfwTerminate();
	return 0;
}

