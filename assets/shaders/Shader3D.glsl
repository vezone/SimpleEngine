#vertex shader
#version 410 core
layout(location = 0) in vec3 i_Position;

out vec4 o_Color;

uniform mat4 u_ViewProjection;

void main()
{
    o_Color = vec4(i_Position, 1.0) + 0.5f;
    gl_Position = u_ViewProjection * vec4(i_Position, 1.0);
}

#fragment shader
#version 410 core
layout(location = 0) out vec4 Color;

in vec4 o_Color;

void main()
{
    Color = vec4(1.0, 1.0, 1.0, 1.0);
}
