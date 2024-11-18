#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float squeezeFactor;

void main()
{
    // Adjust the vertex position to create a squeeze effect
    vec3 squeezedPos = aPos;
    squeezedPos.y += squeezedPos.z * sin(squeezeFactor) / 2;
    squeezedPos.z += squeezedPos.y * sin(squeezeFactor) / 2;

    // Set gl_Position with the adjusted vertex position
    gl_Position = projection * view * model * vec4(squeezedPos, 1.0);

    // Set TexCoord to aTexCoord
    TexCoord = aTexCoord;
}