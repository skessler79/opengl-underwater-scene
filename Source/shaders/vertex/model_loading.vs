#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// const vec4 plane = vec4(0, -1, 0, 1);
uniform vec4 plane;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;         // Inefficient (Should avoid)
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);

    gl_ClipDistance[0] = dot(vec4(FragPos, 1.0), plane);
    // gl_ClipDistance[0] = -0.0;
}