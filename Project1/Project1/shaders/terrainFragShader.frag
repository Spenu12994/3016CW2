#version 460


//Colour value to send to next stage
out vec4 FragColor;

//Colour coordinates from last stage
in vec3 colourFrag;

void main()
{
    FragColor = vec4(colourFrag, 1.0f);
} 