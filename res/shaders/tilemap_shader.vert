#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coord;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(position, 0.0f, 1.0f);
	TexCoord = tex_coord;
}