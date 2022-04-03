#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT
{
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.011;

uniform mat4 projection;
uniform mat4 model;

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;

    // if(model[1][1] < 0.5)
    //     return;

    if(gs_in[index].normal.y < 9.0)
        return;

    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    // GenerateLine(1);
    // GenerateLine(2);

    // for(int i = 0; i < 20; ++i)
    // {
    //     GenerateLine(i);
    // }
}