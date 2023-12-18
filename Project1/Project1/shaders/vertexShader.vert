#version 460
//Triangle position with values retrieved from main.cpp
layout (location = 0) in vec3 position;
//Texture coordinates from last stage
layout (location = 2) in vec2 textureVertex;

//Model-View-Projection Matrix
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

//Texture to send
out vec2 textureFrag;



void main()
{
    //Transformation applied to vertices
    gl_Position = projection * view * model * vec4(position, 1.0f); 
    //Sending texture coordinates to next stage
    textureFrag = textureVertex;
}
