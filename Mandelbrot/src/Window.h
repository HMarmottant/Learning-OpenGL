#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>

class Window
{
private:

    int m_Width, m_Height;
    const char* m_Title;
    bool m_VSync;
    bool Init();

    double presMousePos[2];
    double scroll;

public:

    GLFWwindow* m_Window;

    Window(const char* title, int width, int height, bool vsync);
    ~Window();
    void Clear(float x, float y, float z);
    void Update();
    bool Closed() { return glfwWindowShouldClose(m_Window); };
    void GetMouseScreenDrag(double *returnArray);
    void GetWindowSize(int* width, int* height);

};
