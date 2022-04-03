#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D reflectionTexture;
// uniform sampler2D refractionTexture;

void main()
{
    FragColor = vec4(0.52, 0.76, 0.92, 0.2);

    // vec4 reflectionColour = texture(reflectionTexture, TexCoords);
    // vec4 refractionColour = texture(refractionTexture, TexCoords);

    // FragColor = reflectionColour;
    // FragColor = vec4(vec3(1.0 - texture(reflectionTexture, TexCoords)), 1.0);

    // FragColor = mix(reflectionColour, refractionColour, 0.5);
}