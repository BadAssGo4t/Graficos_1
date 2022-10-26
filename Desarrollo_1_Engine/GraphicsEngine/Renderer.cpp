#include "Renderer.h"


#define ASSERT(x)  if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT (GLLogCall(#x, __FILE__, __LINE__));

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL ERROR] ( " << error << " ):" << function << " : " << file << std::endl;
        return false;
    }
    return true;
}
