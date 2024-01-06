#version 460
layout (location = 0) in vec3 aPos;

//Triangle position with values retrieved from main.cpp
layout (location = 0) in vec3 position;
//Colour coordinates from last stage
layout (location = 1) in vec3 colourVertex;


//Colour to send
out vec3 colourFrag;

out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Position = (view * model * vec4(aPos, 1.0)).xyz;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    colourFrag = colourVertex;
}