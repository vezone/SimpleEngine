#include "OpenGLBase.h"
#include "Utils/Logger.h"
#include "Utils/Types.h"

int
opengl_context_init(GLADloadproc gladLoadProc)
{
    int openGLLoadStatus = gladLoadGLLoader(gladLoadProc);
    if (openGLLoadStatus == 0)
    {
	GERROR("Failed to init GLAD\n");
	return -1;
    }
    GLOG(MAGNETA("OpenGL version %s\n"), glGetString(GL_VERSION));

#if ENGINE_DEBUG == 1
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_error_callback, 0);
#endif
    return 1;
}

void
opengl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    switch (type)
    {
    case GL_NO_ERROR: { break; }
    case GL_INVALID_ENUM:
    {
	GERROR("%s\n", message);
	GERROR("GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.\n");
	break;
    }
    case GL_INVALID_VALUE:
    {
	GERROR("%s\n", message);
	GERROR("GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.\n");
	break;
    }
    case GL_INVALID_OPERATION:
    {
	GERROR("%s\n", message);
	GERROR("GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.\n");
	break;
    }
    case GL_INVALID_FRAMEBUFFER_OPERATION:
    {
	GERROR("%s\n", message);
	GERROR("GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.\n");
	break;
    }
    case GL_OUT_OF_MEMORY:
    {
	GERROR("%s\n", message);
	GERROR("GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n");
	break;
    }
    case GL_STACK_UNDERFLOW:
    {
	GERROR("%s\n", message);
	GERROR("GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow.\n");
	break;
    }
    case GL_STACK_OVERFLOW:
    {
	GWARNING("%s\n", message);
	GERROR("GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow.\n");
	break;
    }
    }
}
