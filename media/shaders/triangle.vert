#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

out vec3 ourColour;

uniform mat4 fullTransformMatrix;

void main()
{
    gl_Position = fullTransformMatrix * (vec4(aPos, 1.0f));
    ourColour = aColour;
}