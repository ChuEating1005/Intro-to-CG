#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// Add material properties
uniform sampler2D ourTexture;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientLight;
uniform vec3 diffuseLight;
uniform vec3 specularLight;
uniform vec3 cameraPos;

void main()
{
    // Retrieve the color from the texture at TexCoord
    vec4 color = texture(ourTexture, TexCoord);
    
    // Ambient
    vec3 ambient = ambientLight * lightColor;

    // Diffuse
    vec3 normVec = normalize(Normal);
    vec3 lightVec = normalize(lightPos - FragPos);
    float diff = max(dot(normVec, lightVec), 0.0);
    vec3 diffuse = diffuseLight * lightColor * diff;

    // Specular
    vec3 viewVec = normalize(cameraPos - FragPos);
    vec3 halfwayVec = normalize(lightVec + viewVec);
    float spec = pow(max(dot(normVec, halfwayVec), 0.0), 8.0);
    vec3 specular = specularLight * lightColor * spec;

    // Combine results
    vec3 result = ambient + diffuse + specular;
    FragColor = clamp(vec4(result, 1.0) * color, 0.0, 1.0);
}