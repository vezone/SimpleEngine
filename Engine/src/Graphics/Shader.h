#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include "OpenGLBase.h"
#include "Utils/SimpleStandardLibrary.h"

typedef struct ShaderSource
{
    char* name;
    const char* vertex_shader;
    const char* fragment_shader;
} ShaderSource;

typedef struct UniformTableType
{
    const char* Key;
    i32 Value;
} UniformTableType;

typedef struct Shader
{
    u32 ShaderID;
    UniformTableType* UniformTable;
} Shader;

ShaderSource shader_load(const char* shader_path);
Shader shader_compile(ShaderSource source);
void shader_delete(Shader* shader);
void shader_bind(Shader* shader);
void shader_unbind();
void shader_delete_collection();

force_inline i32
shader_get_location(Shader* shader, const char* uniformName)
{
    vassert(uniformName != NULL);

    i32 location = shash_geti(shader->UniformTable, uniformName);
    if (location == -1)
    {
	location = glGetUniformLocation(shader->ShaderID, uniformName);
	shash_put(shader->UniformTable, uniformName, location);
    }
    else
    {
	location = shash_get(shader->UniformTable, uniformName);
    }

    return location;
}

force_inline void
shader_set_1float(Shader* shader, const char* uniformName, f32 v0)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform1f(location, v0);
    }
}

force_inline void
shader_set_2float(Shader* shader, const char* uniformName, f32 v0, f32 v1)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform2f(location, v0, v1);
    }
}

force_inline void
shader_set_3float(Shader* shader, const char* uniformName, f32 v0, f32 v1, f32 v2)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform3f(location, v0, v1, v2);
    }
}

force_inline void
shader_set_4float(Shader* shader, const char* uniformName, f32 v0, f32 v1, f32 v2, f32 v3)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform4f(location, v0, v1, v2, v3);
    }
}


force_inline void
shader_set_1int(Shader* shader, const char* uniformName, i32 v0)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform1i(location, v0);
    }
}

force_inline void
shader_set_2int(Shader* shader, const char* uniformName, i32 v0, i32 v1)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform2i(location, v0, v1);
    }
}

force_inline void
shader_set_3int(Shader* shader, const char* uniformName, i32 v0, i32 v1, i32 v2)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform3i(location, v0, v1, v2);
    }
}

force_inline void
shader_set_4int(Shader* shader, const char* uniformName, i32 v0, i32 v1, i32 v2, i32 v3)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform4i(location, v0, v1, v2, v3);
    }
}

force_inline void
shader_set_1uint(Shader* shader, const char* uniformName, u32 v0)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform1ui(location, v0);
    }
}

force_inline void
shader_set_2uint(Shader* shader, const char* uniformName, u32 v0, u32 v1)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform2ui(location, v0, v1);
    }
}

force_inline void
shader_set_3uint(Shader* shader, const char* uniformName, u32 v0, u32 v1, u32 v2)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform3ui(location, v0, v1, v2);
    }
}

force_inline void
shader_set_4uint(Shader* shader, const char* uniformName, u32 v0, u32 v1, u32 v2, u32 v3)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform4ui(location, v0, v1, v2, v3);
    }
}


force_inline void
shader_set_float1(Shader* shader, const char* uniformName, i32 count, f32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform1fv(location, count, values);
    }
}

force_inline void
shader_set_float2(Shader* shader, const char* uniformName, i32 count, f32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform2fv(location, count, values);
    }
}

force_inline void
shader_set_float3(Shader* shader, const char* uniformName, i32 count, f32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform3fv(location, count,  values);
    }
}

force_inline void
shader_set_float4(Shader* shader, const char* uniformName, i32 count, f32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform4fv(location, count, values);
    }
}

force_inline void
shader_set_int1(Shader* shader, const char* uniformName, i32 count, i32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform1iv(location, count, values);
    }
}

force_inline void
shader_set_int2(Shader* shader, const char* uniformName, i32 count, i32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform2iv(location, count, values);
    }
}

force_inline void
shader_set_int3(Shader* shader, const char* uniformName, i32 count, i32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform3iv(location, count, values);
    }
}

force_inline void
shader_set_int4(Shader* shader, const char* uniformName, i32 count, i32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform4iv(location, count, values);
    }
}

force_inline void
shader_set_uint1(Shader* shader, const char* uniformName, i32 count, u32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform4uiv(location, count, values);
    }
}

force_inline void
shader_set_uint2(Shader* shader, const char* uniformName, i32 count, u32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform2uiv(location, count, values);
    }
}

force_inline void
shader_set_uint3(Shader* shader, const char* uniformName, i32 count, u32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform3uiv(location, count, values);
    }
}

force_inline void
shader_set_uint4(Shader* shader, const char* uniformName, i32 count, u32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniform4uiv(location, count, values);
    }
}

force_inline void
shader_set_mat2(Shader* shader, const char* uniformName, i32 count, i8 transpose, f32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniformMatrix2fv(location, count, transpose, values);
    }
}

force_inline void
shader_set_m3(Shader* shader, const char* uniformName, i32 count, i8 transpose, f32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniformMatrix3fv(location, count, transpose, values);
    }
}

force_inline void
shader_set_m4(Shader* shader, const char* uniformName, i32 count, i8 transpose, f32* values)
{
    i32 location = shader_get_location(shader, uniformName);
    if (location >= 0)
    {
	glUniformMatrix4fv(location, count, transpose, values);
    }
}

#endif
