#include "Window.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

GLFWwindow* aux;

Window::Window()
{
    window = aux;
}

Window::Window(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, title, monitor, share);
}

void Window::WindowContext()
{
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
}

bool Window::ShouldClose()
{
    return !glfwWindowShouldClose(window);
}

GLFWwindow* Window::GetWindow()
{
    return window;
}
void Window::ClearBackground()
{
    /* Start Render here */ //Clear the Background
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
void Window::SwapBuffer()
{
    /* Swap front and back buffers */
    glfwSwapBuffers(window);
}
