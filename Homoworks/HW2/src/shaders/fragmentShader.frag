#version 330 core
out vec4 FragColor;

in vec2 TexCoord; 

uniform sampler2D ourTexture;
uniform vec3 rainbowColor;
uniform bool useRainbowColor;
uniform vec3 cubeColor;
uniform bool useHelicopter;

void main()
{
    // Retrieve the color from the texture at TexCoord
    vec4 color = texture(ourTexture, TexCoord);

    // Calculate the dot product and set FragColor
    if (useRainbowColor) {
        float r = dot(color.r, rainbowColor.r);
        float g = dot(color.g, rainbowColor.g);
        float b = dot(color.b, rainbowColor.b);
        FragColor = vec4(r, g, b, color.a);
    }
    else if (useHelicopter) {
        FragColor = vec4(cubeColor, 1.0);
    }
    else {
        FragColor = color;
    }
} 