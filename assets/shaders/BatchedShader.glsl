#vertex shader
#version 440 core
layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec4 i_Color;
layout(location = 2) in vec2 i_TextureCoordinates;
layout(location = 3) in float i_TextureId;
layout(location = 4) in int i_EntityID;

out vec4 v_Color;
out vec2 v_TextureCoordinates;
out float v_TextureId;
out int v_EntityID;
uniform mat4 u_ViewProjection;

void main()
{
    v_Color = i_Color;
    v_TextureCoordinates = i_TextureCoordinates;
    //v_TextureCoordinates.y = 1.0 - v_TextureCoordinates.y;
    v_TextureId = i_TextureId;
    v_EntityID = i_EntityID;
    gl_Position = u_ViewProjection * vec4(i_Position, 1.0);
}

#fragment shader
#version 440 core
layout(location = 0) out vec4 Color;

in vec4 v_Color;
in vec2 v_TextureCoordinates;
in float v_TextureId;
in int v_EntityID;
uniform sampler2D u_Textures[32];

void main()
{
    if (v_TextureId == 0)
    {
	Color = v_Color;
    }
    else
    {
	Color = v_Color * (texture(u_Textures[int(v_TextureId)], v_TextureCoordinates));
    }
}
