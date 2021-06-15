#include "Shader.h"

#include <stdlib.h>
#include <stdio.h>
#include "Utils/IO.h"
#include "Utils/String.h"
#include "Utils/Array.h"
#include "Utils/Logger.h"

Shader* g_Shaders = NULL;

ShaderSource
shader_load(const char* shader_path)
{
    i32 vertex_index;
    i32 fragment_index;
    i32 vertex_keyword_length;
    i32 fragment_keyword_length;
    char* shader_source;
    char* shader_name;
    const char* vertex_shader_source;
    const char* fragment_shader_source;
    ShaderSource source = {};

    shader_source = file_read_string(shader_path);
    shader_name = file_get_name_with_extension(shader_path);
    SHADERDEBUG(CYAN("shader source: %s\n"), shader_source);
    if (shader_source == NULL)
    {
	GERROR("shader file open error!");
	return (ShaderSource) { };
    }

    vertex_index = vstring_index_of_string(shader_source, "#vertex shader");
    fragment_index = vstring_index_of_string(shader_source, "#fragment shader");
    vertex_keyword_length = vstring_length("#vertex shader");
    fragment_keyword_length = vstring_length("#fragment shader");

    vertex_shader_source = vstring_substring_range(shader_source, vertex_index + vertex_keyword_length + 1, (fragment_index - 1));
    fragment_shader_source = vstring_substring(shader_source, fragment_index + fragment_keyword_length + 1);

    SHADERDEBUG("vertex shader:\n"YELLOW("%s\n"), vertex_shader_source);
    SHADERDEBUG("fragment shader:\n"GREEN("%s\n"), fragment_shader_source);

    source.name = shader_name;
    source.vertex_shader   = (char*) vertex_shader_source;
    source.fragment_shader = (char*) fragment_shader_source;

    if (shader_source)
    {
	free(shader_source);
    }

    return source;
}

static void
shader_error_check(u32 shaderID)
{
    i32 isCompiled = GL_FALSE;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
	i32 logLength = 0;
	char errorMessage[1024];
	glGetShaderInfoLog(shaderID, 1024, &logLength, errorMessage);
	if (logLength > 0)
	{
	    GERROR("shader_error_check[is_compiled: %d, log_length:%d]: %s\n", isCompiled, logLength, errorMessage);
	}
    }
}

Shader
shader_compile(ShaderSource source)
{
    Shader shader = {};

    u32 vertex_shader_id;
    u32 fragment_shader_id;
    u32 shader_program_id;

    SHADERDEBUG(GREEN5("SHADER")" %s\n", source.name);

    if (source.vertex_shader == NULL || source.fragment_shader == NULL || vstring_length(source.vertex_shader) < 0 || vstring_length(source.fragment_shader) < 0)
    {
	GERROR("Shader source is not loaded correctly!\n");
	return (Shader) { .ShaderID = -1, .UniformTable = NULL };
    }

    SHADERDEBUG(GREEN("Compiling")" vertex shader\n");
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &source.vertex_shader, 0);
    glCompileShader(vertex_shader_id);
    shader_error_check(vertex_shader_id);

    SHADERDEBUG(GREEN("Compiling")" fragment shader\n");
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &source.fragment_shader, 0);
    glCompileShader(fragment_shader_id);
    shader_error_check(fragment_shader_id);

    SHADERDEBUG(GREEN("Linking")" program\n");
    shader_program_id = glCreateProgram();
    SHADERDEBUG("SHADER ID: %d\n", shader_program_id);
    glAttachShader(shader_program_id, vertex_shader_id);
    glAttachShader(shader_program_id, fragment_shader_id);
    glLinkProgram(shader_program_id);
    shader_error_check(shader_program_id);

    glDetachShader(shader_program_id, vertex_shader_id);
    glDetachShader(shader_program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    array_push(g_Shaders, shader);

    return (Shader) { .ShaderID = shader_program_id, .UniformTable = NULL };
}

void
shader_delete(Shader* shader)
{
    glDeleteProgram(shader->ShaderID);
    shash_free(shader->UniformTable);
}

void
shader_bind(Shader* shader)
{
    glUseProgram(shader->ShaderID);
}

void
shader_unbind()
{
    glUseProgram(0);
}

void
shader_delete_collection()
{
    GDEBUG(GREEN("Begin: delete collection of shaders\n"));

    if (g_Shaders == NULL)
    {
	return;
    }

    Shader shader;
    i32 i, count = array_count(g_Shaders);
    for (i = 0; i < count; i++)
    {
	shader = g_Shaders[i];
	shader_delete(&shader);
    }

    GDEBUG(GREEN("End: delete collection of shaders\n"));
}
