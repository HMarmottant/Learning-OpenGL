#include "Window.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool Window::Init()
{
	if (!glfwInit())
	{
		std::cout << "ERROR::GLFW::INIT : GLFW failed to initialize" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
	glfwSetKeyCallback(m_Window, key_callback);
	glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);

	glfwMakeContextCurrent(m_Window);

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		std::cout << "ERROR::GLEW::INIT : GLEW failed to initialize" << std::endl;
		return false;
	}

	if (m_VSync)
	{
		glfwSwapInterval(1);
		std::cout << "WINDOW::OPTIONS::VSYNC::ON" << std::endl;
	}
	else
	{
		std::cout << "WINDOW::OPTIONS::VSYNC::OFF" << std::endl;
	}
	return true;
}

Window::Window(const char* title, int width, int height, bool vsync)
{
	m_Title = title;
	m_Width = width;
	m_Height = height;
	m_VSync = vsync;
	presMousePos[0] = 0;
	presMousePos[1] = 0;
	Init();
}

void Window::Clear(float x, float y, float z)
{
	glClearColor(x, y, z, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Update()
{
	int width, height;

	glfwGetFramebufferSize(m_Window, &width, &height);

	glViewport(0, 0, width, height);

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Window::GetMouseScreenDrag(double* returnArray)
{
	double xpos, ypos;
	glfwGetCursorPos(m_Window, &xpos, &ypos);
	int state = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		returnArray[0] += (presMousePos[0] - xpos) ;
		returnArray[1] += (ypos - presMousePos[1]);
	}
	presMousePos[0] = xpos;
	presMousePos[1] = ypos;
}

Window::~Window()
{
	glfwTerminate();
	glfwDestroyWindow(m_Window);
}