
//GLAD
#include <glad/glad.h>


//glfw
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//LEARNOPENGL
#include <learnopengl/shader_m.h>

//STD
#include <iostream>




//GENERAL
#include "model.h"
#include "main.h"
#include "FastNoiseLite.h"
#include <random>


using namespace std;
using namespace glm;



//Window
int windowWidth;
int windowHeight;


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
vec3 cameraPosition = vec3(0.0f, 5.0f, 3.0f);
//The direction of travel
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
//Up position within world space
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

//Camera sidways rotation
float cameraYaw = -90.0f;
//Camera vertical rotation
float cameraPitch = 0.0f;
//Determines if first entry of mouse into window
bool mouseFirstEntry = true;
//Positions of camera from given last frame
float cameraLastXPos = 800.0f / 2.0f;
float cameraLastYPos = 600.0f / 2.0f;

//Model-View-Projection Matrix
mat4 mvp;
mat4 model;
mat4 view;
mat4 projection;

//Time
//Time change
float deltaTime = 0.0f;
//Last value of time change
float lastFrame = 0.0f;

#define RENDER_DISTANCE 128 //Render width of map
#define MAP_SIZE (RENDER_DISTANCE * RENDER_DISTANCE) //Size of map in x & z space

//Amount of chunks across one dimension
const int squaresRow = RENDER_DISTANCE - 1;
//Two triangles per square to form a 1x1 chunk
const int trianglesPerSquare = 2;
//Amount of triangles on map
const int trianglesGrid = squaresRow * squaresRow * trianglesPerSquare;

float terrainSize = 5.0f;

int main()
{
    //Initialisation of GLFW
    glfwInit();
    //Initialisation of 'GLFWwindow' object
    windowWidth = 1280;
    windowHeight = 720;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Lab5", NULL, NULL);

    //Checks if window has been successfully instantiated
    if (window == NULL)
    {
        cout << "GLFW Window did not instantiate\n";
        glfwTerminate();
        return -1;
    }

    //Sets cursor to automatically bind to window & hides cursor pointer
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Binds OpenGL to window
    glfwMakeContextCurrent(window);

    //Initialisation of GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD failed to initialise\n";
        return -1;
    }


    //Loading of shaders
    Shader terrainShader("shaders/terrainVertShader.vert", "shaders/terrainFragShader.frag");
    Shader Shaders("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    
    Model Rock("media/rock/Rock07-Base.obj");
    Model Sign("media/Signature/untitled.obj");
    Model Tree("media/Tree/tree.obj");

    Shaders.use();
    Shaders.setVec3("albedo", 0.5f, 0.0f, 0.0f);
    Shaders.setFloat("ao", 1.0f);


    // lights
    // ------
    glm::vec3 lightPositions[] = {
        glm::vec3(0.0f, 0.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(1500.0f, 1500.0f, 1500.0f),
    };






    //Sets the viewport size within the window to match the window size of 1280x720
    glViewport(0, 0, 1280, 720);

    //Sets the framebuffer_size_callback() function as the callback for the window resizing event
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Sets the mouse_callback() function as the callback for the mouse movement event
    glfwSetCursorPosCallback(window, mouse_callback);



    //Projection matrix
    projection = perspective(radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);


    //Procedural Terrain----

    //Assigning perlin noise type for map
    FastNoiseLite TerrainNoise;
    //Setting noise type to Perlin
    TerrainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    //Sets the noise scale
    TerrainNoise.SetFrequency(0.05f);
    //Generates a random seed between integers 0 & 100

    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0, 100);

    int randTerSeed = dist(mt);

    int terrainSeed = randTerSeed;
    //Sets seed for noise
    TerrainNoise.SetSeed(terrainSeed);


    int randBioSeed = dist(mt);
    //Biome noise
    FastNoiseLite BiomeNoise;
    BiomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    BiomeNoise.SetFrequency(0.05f);
    int biomeSeed = randBioSeed;
    TerrainNoise.SetSeed(biomeSeed);

    // load and create a texture
// -------------------------
// load image, create texture and generate mipmaps
// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;




    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    
    //Generation of height map vertices
    GLfloat terrainVertices[MAP_SIZE][6];

    //Terrain vertice index
    int i = 0;
    //Using x & y nested for loop in order to apply noise 2-dimensionally
    for (int y = 0; y < RENDER_DISTANCE; y++)
    {
        for (int x = 0; x < RENDER_DISTANCE; x++)
        {
            //Setting of height from 2D noise value at respective x & y coordinate
            terrainVertices[i][1] = TerrainNoise.GetNoise((float)x, (float)y);

            //Retrieval of biome to set
            float biomeValue = BiomeNoise.GetNoise((float)x, (float)y);

            if (biomeValue <= -0.75f) //Plains
            {
                terrainVertices[i][3] = 0.0f;
                terrainVertices[i][4] = 0.75f;
                terrainVertices[i][5] = 0.25f;
            }
            else //Desert
            {
                terrainVertices[i][3] = 1.0f;
                terrainVertices[i][4] = 1.0f;
                terrainVertices[i][5] = 0.5f;
            }

            i++;
        }
    }

    //Positions to start drawing from
    float drawingStartPosition = 1.0f;
    float columnVerticesOffset = drawingStartPosition;
    float rowVerticesOffset = drawingStartPosition;

    int rowIndex = 0;
    for (int i = 0; i < MAP_SIZE; i++)
    {
        //Generation of x & z vertices for horizontal plane
        terrainVertices[i][0] = columnVerticesOffset;
        terrainVertices[i][2] = rowVerticesOffset;

        //Determination of biomes based on height
        if (terrainVertices[i][1] >= (0.5f / 8.0f))
        {
            //Snow
            terrainVertices[i][3] = 1.0f;
            terrainVertices[i][4] = 1.0f;
            terrainVertices[i][5] = 1.0f;
        }
        if (terrainVertices[i][1] <= (-0.3f))
        {
            //water
            terrainVertices[i][3] = 0.161f;
            terrainVertices[i][4] = 0.525f;
            terrainVertices[i][5] = 0.988f;
        }
        //Shifts x position across for next triangle along grid
        columnVerticesOffset = columnVerticesOffset + -0.0625f;

        //Indexing of each chunk within row
        rowIndex++;
        //True when all triangles of the current row have been generated
        if (rowIndex == RENDER_DISTANCE)
        {
            //Resets for next row of triangles
            rowIndex = 0;
            //Resets x position for next row of triangles
            columnVerticesOffset = drawingStartPosition;
            //Shifts y position
            rowVerticesOffset = rowVerticesOffset + -0.0625f;
        }
    }

    //Generation of height map indices
    GLuint terrainIndices[trianglesGrid][3];

    //Positions to start mapping indices from
    int columnIndicesOffset = 0;
    int rowIndicesOffset = 0;

    //Generation of map indices in the form of chunks (1x1 right angle triangle squares)
    rowIndex = 0;
    for (int i = 0; i < trianglesGrid - 1; i += 2)
    {
        terrainIndices[i][0] = columnIndicesOffset + rowIndicesOffset; //top left
        terrainIndices[i][2] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom left
        terrainIndices[i][1] = 1 + columnIndicesOffset + rowIndicesOffset; //top right

        terrainIndices[i + 1][0] = 1 + columnIndicesOffset + rowIndicesOffset; //top right
        terrainIndices[i + 1][2] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom left
        terrainIndices[i + 1][1] = 1 + RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom right

        //Shifts x position across for next chunk along grid
        columnIndicesOffset = columnIndicesOffset + 1;

        //Indexing of each chunk within row
        rowIndex++;

        //True when all chunks of the current row have been generated
        if (rowIndex == squaresRow)
        {
            //Resets for next row of chunks
            rowIndex = 0;
            //Resets x position for next row of chunks
            columnIndicesOffset = 0;
            //Shifts y position
            rowIndicesOffset = rowIndicesOffset + RENDER_DISTANCE;
        }
    }

    //Sets index of VAO
    glGenVertexArrays(NumVAOs, VAOs);
    //Binds VAO to a buffer
    glBindVertexArray(VAOs[0]);
    //Sets indexes of all required buffer objects
    glGenBuffers(NumBuffers, Buffers);
    //glGenBuffers(1, &EBO);

    //Binds vertex object to array buffer
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
    //Allocates buffer memory for the vertices of the 'Triangles' buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

    //Binding & allocation for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[Indices]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terrainIndices), terrainIndices, GL_STATIC_DRAW);

    //Allocation & indexing of vertex attribute memory for vertex shader
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Colours
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Unbinding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    


    //----------------------------------------------------------------------------------------------------------------------------------------
    // 
    //Code for adding tree instances
    unsigned int treeAmount = 1000;
    glm::mat4* modelTreeMatrices;
    modelTreeMatrices = new glm::mat4[treeAmount];
    srand(glfwGetTime()); // initialize random seed	
    for (unsigned int i = 0; i < treeAmount; i++)
    {
        //origin variables
        //set these to be added at the end of translate calculations to shift the tree origin to a specific point
        float originx = 0.0f;
        float originy = 0.0f;
        float originz = 0.0f;

        random_device rd;
        mt19937 mt(rd());
        uniform_real_distribution<double> dist(0, MAP_SIZE);

        int verticeLocal = dist(mt);


        glm::mat4 model = glm::mat4(1.0f);
        float x = (terrainVertices[verticeLocal][0]) * terrainSize;
        float y = (terrainVertices[verticeLocal][1]) * terrainSize;
        float z = (terrainVertices[verticeLocal][2]) * terrainSize;
        model = glm::translate(model, glm::vec3(x + originx, y + originy, z + originz));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 5) / 1000.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector


        // 4. now add to list of matrices
        modelTreeMatrices[i] = model;
    }

    //Code for adding rock isntances
     //Code for adding rocks
    unsigned int amount = 1000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // initialize random seed	
    float radius = 5.0;//how wide is our circle
    float offset = 1.0f;//how far from our circle can the object deviate (higher = more scattered, lower = more circular)
    for (unsigned int i = 0; i < amount; i++)
    {
        //origin variables
        //set these to be added at the end of translate calculations to shift the tree origin to a specific point
        float originx = 0.0f;
        float originy = 0.0f;
        float originz = 0.0f;

        random_device rd;
        mt19937 mt(rd());
        uniform_real_distribution<double> dist(0, MAP_SIZE);

        int verticeLocal = dist(mt);


        verticeLocal = verticeLocal - (i*10);
        if (verticeLocal < 0) {
            verticeLocal = rand() % MAP_SIZE + i*10;
        }
        if (verticeLocal > MAP_SIZE) {
            verticeLocal = MAP_SIZE % verticeLocal;
        }


        glm::mat4 model = glm::mat4(1.0f);
        float x = (terrainVertices[verticeLocal][0]) * terrainSize;
        float y = (terrainVertices[verticeLocal][1]) * terrainSize;
        float z = (terrainVertices[verticeLocal][2]) * terrainSize;
        model = glm::translate(model, glm::vec3(x + originx, y + originy, z + originz));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 40) / 5000.0);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    //Skybox Cubemap------------------------------------------------------------------------------




    //pbr lighting
    float lightIterate = 10;
    float lightX = 0;
    float lightY = 100;
    int lightHeightDiff = 100; 
    int lightDirX = 0;//0=left 1= right
    int lightDirY = 0; //0=down 1=up

    float signatureRotate = 0;

    //Render loop
    while (glfwWindowShouldClose(window) == false)
    {
        //Time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Input
        ProcessUserInput(window); //Takes user input

        //Rendering
        glClearColor(0.25f, 0.0f, 1.0f, 1.0f); //Colour to display on cleared window
        glClear(GL_COLOR_BUFFER_BIT); //Clears the colour buffer
        glClear(GL_DEPTH_BUFFER_BIT); //Might need
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);


        //Terrain----------------------------------------------------
        terrainShader.use();
        model = mat4(1.0f);
        model = glm::scale(model, glm::vec3(terrainSize));
        view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction
        terrainShader.setMat4("projection", projection);
        terrainShader.setMat4("view", view);
        terrainShader.setMat4("model", model);

        //Drawing
        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, MAP_SIZE * 32, GL_UNSIGNED_INT, 0);

        // view/projection transformations
        view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction


        //Model matrix
        model = mat4(1.0f);
        //Scaling to zoom in
        model = scale(model, vec3(2.0f));
        //Looking straight forward
        model = rotate(model, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
        //Elevation to look upon terrain
        model = translate(model, vec3(0.0f, 3.f, -1.0f));

        signatureRotate += 0.05f;
        model = rotate(model, radians(signatureRotate), vec3(0.0f, 1.0f, 0.0f));

        //Transformations & Drawing
        //Viewer orientation
        view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction
        Shaders.use();
        Shaders.setMat4("projection", projection);
        Shaders.setMat4("view", view);
        Shaders.setMat4("model", model);

        //draw signature
        Sign.Draw(Shaders);

        //Rock (changes MVP in relation to past values)
        for (unsigned int i = 0; i < amount; i++)
        {            
            Shaders.setMat4("model", modelMatrices[i]);
            Rock.Draw(Shaders);//PUT ANY MODEL HERE TO HAVE THE INSTANCING RUN OVER IT
        }

        //load Trees 
        for (unsigned int i = 0; i < treeAmount; i++)
        {
            Shaders.setMat4("model", modelTreeMatrices[i]);
            Tree.Draw(Shaders);//PUT ANY MODEL HERE TO HAVE THE INSTANCING RUN OVER IT
        }
        //move Light as sun
        
        float lightRadius = 20; //higher = further from world centre
        lightPositions[0] = vec3((lightX/ 1), (lightY/ 20), (-5.0f));

        float lightDiff = 0.1f; // the higher the number, the slower the sun rotates


        if (lightDirX == 0) {
            lightX = lightX - lightDiff;
        }
        else {
            lightX = lightX + lightDiff;
        }

        if (lightDirY == 0) {
            //lightY = lightY - lightDiff;
        }
        else {
            //lightY = lightY + lightDiff;
        }

        if (lightX >= lightRadius) {
            lightDirX = 0;
        }
        else if (lightX <= (lightRadius * -1)) {
            lightDirX = 1;
        }

        if (lightY >= lightRadius) {
            lightDirY = 0;
        }
        else if (lightY <= (lightRadius * -1)) {
            lightDirY = 1;
        }



        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        // keeps the codeprint small.
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            Shaders.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            Shaders.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.005f));
            Shaders.setMat4("model", model);
            Shaders.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            
        }
        model = scale(model, vec3(0.5f));
        Shaders.setMat4("model", model);
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //Initially no last positions, so sets last positions to current positions
    if (mouseFirstEntry)
    {
        cameraLastXPos = (float)xpos;
        cameraLastYPos = (float)ypos;
        mouseFirstEntry = false;
    }

    //Sets values for change in position since last frame to current frame
    float xOffset = (float)xpos - cameraLastXPos;
    float yOffset = cameraLastYPos - (float)ypos;

    //Sets last positions to current positions for next frame
    cameraLastXPos = (float)xpos;
    cameraLastYPos = (float)ypos;

    //Moderates the change in position based on sensitivity value
    const float sensitivity = 0.025f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    //Adjusts yaw & pitch values against changes in positions
    cameraYaw += xOffset;
    cameraPitch += yOffset;

    //Prevents turning up & down beyond 90 degrees to look backwards
    if (cameraPitch > 89.0f)
    {
        cameraPitch = 89.0f;
    }
    else if (cameraPitch < -89.0f)
    {
        cameraPitch = -89.0f;
    }

    //Modification of direction vector based on mouse turning
    vec3 direction;
    direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
    direction.y = sin(radians(cameraPitch));
    direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
    cameraFront = normalize(direction);
}

void ProcessUserInput(GLFWwindow* WindowIn)
{
    //Closes window on 'exit' key press
    if (glfwGetKey(WindowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(WindowIn, true);
    }

    //Extent to which to move in one instance
    const float movementSpeed = 1.0f * deltaTime;
    //WASD controls
    if (glfwGetKey(WindowIn, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPosition += movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPosition -= movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPosition -= normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPosition += normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_F) == GLFW_PRESS)
    {

    }
}

//function to load and set cubemap
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}



//shouldnt be used!
void SetMatrices(Shader& ShaderProgramIn)
{
    mvp = projection * view * model; //Setting of MVP
    ShaderProgramIn.setMat4("model", mvp); //Setting of uniform with Shader class
}