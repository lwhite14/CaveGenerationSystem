#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aInstancePosition;

out vec2 TexCoord;

uniform mat4 fullTransformMatrix;

void main()
{
    gl_Position = fullTransformMatrix * (vec4(aPos, 1.0f) + vec4(aInstancePosition, 1.0f));
    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}