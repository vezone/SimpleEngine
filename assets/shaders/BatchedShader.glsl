#vertex shader
#version 410 core
layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec4 i_Color;
layout(location = 2) in vec2 i_TextureCoordinates;
layout(location = 3) in float i_TextureId;
layout(location = 3) in float i_EntityID;

out vec4 o_Color;
out vec2 o_TextureCoordinates;
out float o_TextureId;

uniform mat4 u_ViewProjection;

void main()
{
    o_Color = i_Color;
    o_TextureCoordinates = i_TextureCoordinates;
    //o_TextureCoordinates.y = 1.0 - o_TextureCoordinates.y;
    o_TextureId = i_TextureId;
    gl_Position = u_ViewProjection * vec4(i_Position, 1.0);
}

#fragment shader
#version 410 core
layout(location = 0) out vec4 Color;

in vec4 o_Color;
in vec2 o_TextureCoordinates;
in float o_TextureId;

uniform sampler2D u_Textures[32];

void main()
{
    vec4 color = o_Color * (texture(u_Textures[int(o_TextureId)], o_TextureCoordinates * o_TextureId));
    Color = color;
}
