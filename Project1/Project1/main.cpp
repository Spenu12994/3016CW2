//STD
#include <iostream>

//TEXTURING
#include "stb_image.h"

//GLAD
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "main.h"

//GLM
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//LEARNOPENGL
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>






using namespace std;

GLuint program;

//VAO vertex attribute positions in correspondence to vertex attribute type
enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 };
//VAOs
GLuint VAOs[NumVAOs];

//Buffer types
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
//Buffer objects
GLuint Buffers[NumBuffers];


//Transformations
//Relative position within world space
vec3 cameraPosition = vec3(0.0f, 0.0f, 3.0f);
//The direction of travel
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
//Up position within world space
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

//Time
//Time change
float deltaTime = 0.0f;
//Last value of time change
float lastFrame = 0.0f;



int main()
{
    //Initialisation of GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD failed to initialise\n";
        return -1;
    }

    //Loading of shaders
    Shader Shaders("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    Model Rock("media/Toilet.obj");
    Shaders.use();

    //Initialisation of 'GLFWwindow' object
    const int windowWidth = 1280;
    const int windowHeight = 720;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Lab5", NULL, NULL);
    

    //Checks if window has been successfully instantiated
    if (window == NULL)
    {
        cout << "GLFW Window did not instantiate\n";
        glfwTerminate();
        return -1;
    }

    //Binds OpenGL to window
    glfwMakeContextCurrent(window);


    glUseProgram(program);

    //Sets the viewport size within the window to match the window size of 1280x720
    glViewport(0, 0, 1280, 720);

    //Sets the framebuffer_size_callback() function as the callback for the window resizing event
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    float vertices[] = {
    -0.5f, -0.5f, 0.0f, //pos 0 | x, y, z
    0.5f, -0.5f, 0.0f, //pos 1
    0.0f, 0.5f, 0.0f //pos 2
    };


    //Model matrix
    mat4 model = mat4(1.0f);
    //Scaling to zoom in
    model = scale(model, vec3(2.0f, 2.0f, 2.0f));
    //Rotation to look down
    model = rotate(model, radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
    //Movement to position further back
    model = translate(model, vec3(0.0f, 1.f, -1.5f));

    //Projection matrix
    mat4 projection = perspective(radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    //Render loop
    while (glfwWindowShouldClose(window) == false)
    {
        //Input
        ProcessUserInput(window); //Takes user input

        //Rendering
        glClearColor(0.25f, 0.0f, 1.0f, 1.0f); //Colour to display on cleared window
        glClear(GL_COLOR_BUFFER_BIT); //Clears the colour buffer
        glClear(GL_DEPTH_BUFFER_BIT); //Might need

        glEnable(GL_CULL_FACE); //Discards all back-facing triangles

        //Transformations
        mat4 view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction
        mat4 mvp = projection * view * model;
        Shaders.setMat4("mvpIn", mvp); //Setting of uniform with Shader class

        //Drawing
        Rock.Draw(Shaders);

        //Refreshing
        glfwSwapBuffers(window); //Swaps the colour buffer
        glfwPollEvents(); //Queries all GLFW events
    }

    //Safely terminates GLFW
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //Resizes window based on contemporary width & height values
    glViewport(0, 0, width, height);
}

void ProcessUserInput(GLFWwindow* WindowIn)
{
    //Closes window on 'exit' key press
    if (glfwGetKey(WindowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(WindowIn, true);
    }
}