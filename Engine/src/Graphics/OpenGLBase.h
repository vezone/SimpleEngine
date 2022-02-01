#ifndef OPENGLBASE
#define OPENGLBASE

#include <glad/glad.h>
#include "Utils/SimpleStandardLibrary.h"

int opengl_context_init(GLADloadproc gladLoadProc);
void opengl_error_callback(GLenum source, GLenum error, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);


#endif
