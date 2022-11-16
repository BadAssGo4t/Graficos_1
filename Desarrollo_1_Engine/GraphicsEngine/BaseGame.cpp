#include "BaseGame.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "VertexArray.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>



static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL ERROR] ( " << error << " ):" << function << " : " << file << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource //Shader goes into Renderer
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) // pre-shader
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, Fragment = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos) //set to vertex
                type = ShaderType::VERTEX;

            else if (line.find("fragment") != std::string::npos) //set to fragment
                type = ShaderType::Fragment;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}


static unsigned int CompileShader(unsigned int type, const std::string& source) //shader
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE)
    {
        int lenght;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght));
        char* message = (char*)alloca(lenght * sizeof(char));
        GLCall(glGetShaderInfoLog(id, lenght, &lenght, message));
        std::cout << "failed to compile " << (type == GL_VERTEX_SHADER ? " vertex" : " fragment");

        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}
// ^
// "Writing a shader in openGL" - cherno (vid)
// v
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDetachShader(program, vs));
    GLCall(glDeleteShader(vs));
    GLCall(glDetachShader(program, fs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    // Init lib
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context 
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    glfwSwapInterval(2);


    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current 
    glfwMakeContextCurrent(window);

    // Glew Init.
    if (glewInit() != GLEW_OK)
        std::cout << "ERROR!" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    {
    // triangle load
    float positions[] =
    {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    // VAO - Vertex array ---------------
    unsigned int vao; 
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));
    // --  --  --  --  --  --  --  --  --

    // Call to Vertex Buffer
    VertexArray va;
    VertexBuffer vb(positions, 4 * 2 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    //Index Buffer Object = ibo
    IndexBuffer ib(indices, 6);

    ShaderProgramSource source = ParseShader("../GraphicsEngine/Shader.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    GLCall(int location = glGetUniformLocation(shader, "u_Color"));//Shader color - Uniform
    ASSERT(location != -1);
    glUniform4f(location, 0.5f, 0.3f, 0.9f, 1.0f);
    float r = 0.0f;  float g = 0.0f; float b = 0.0f;
    float increment = 0.5f; // End Shader color - Uniform

    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));



    // Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Render here 
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        // Draw Binded buffer
        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, 0.3f, 0.9f, 1.0f)); //uniform color

        va.Bind();
        ib.bind(); // Bind Index buffer

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r > 1.0f)  // Swaps color while running
            increment = -0.5f;
        else if (r < 0.0f)
            increment = 0.05f;
        r += increment;

        // Swap front and back buffers 
        GLCall(glfwSwapBuffers(window));

        // Poll for and process events 
        GLCall(glfwPollEvents());
    }

    GLCall(glDeleteProgram(shader));

    }
    glfwTerminate();
    return 0;
}


