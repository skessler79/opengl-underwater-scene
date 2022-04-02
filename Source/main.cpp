#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "include/stb_image.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);
unsigned int loadCubemap(std::vector<std::string> faces);

// Window Settings
// ---------------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera Settings
// ---------------
Camera camera(glm::vec3(0.0f, 2.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Spotlight Settings
// ------------------
bool spotlightToggle = false;
bool spotlightToggleReleased = true;

// Frame Timing
// ------------
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // Initialize GLFW and configure GLFW
    // ----------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window object via GLFW
    // -------------------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Make GLFW Context
    glfwMakeContextCurrent(window);

    // Setup Callbacks
    // ---------------
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture our mouse with GLFW
    // ---------------------------
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD (Load all OpenGL function pointers)
    // ---------------------------------------------------
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Tell stb_image.h to flip loaded textures on the y-axis (before loading model)
    stbi_set_flip_vertically_on_load(true);

    // -----------------------------
    // Configure global OpenGL state
    // -----------------------------
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // ------------------------------------
    // Build and compile our shader program
    // ------------------------------------
    Shader ourShader("model_loading.vs", "model_loading.fs");
    Shader lightCubeShader("light_cube.vs", "light_cube.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");
    Shader waterShader("water_shader.vs", "water_shader.fs");
    Shader screenShader("framebuffers_screen.vs", "framebuffers_screen.fs");

    Shader cubeShader("colors.vs", "colors.fs");

    // Set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // Skybox
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    float waterVertices[] =
    {
        // positions        // texture coords
        -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         1.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         1.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         1.0f,  0.0f,  1.0f,  1.0f,  1.0f
    };

    // Vertex attributes for a the refraction quad that fills the top right
    float reflectionVertices[] = {
        // positions   // texCoords
        -0.9f,  1.0f,  0.0f, 1.0f,
        -0.9f,  0.4f,  0.0f, 0.0f,
        -0.1f,  0.4f,  1.0f, 0.0f,

        -0.9f,  1.0f,  0.0f, 1.0f,
        -0.1f,  0.4f,  1.0f, 0.0f,
        -0.1f,  1.0f,  1.0f, 1.0f
    };

    float refractionVertices[] = 
    {
        // positions   // texCoords
         0.9f,  1.0f,  0.0f, 1.0f,
         0.9f,  0.4f,  0.0f, 0.0f,
         0.1f,  0.4f,  1.0f, 0.0f,

         0.9f,  1.0f,  0.0f, 1.0f,
         0.1f,  0.4f,  1.0f, 0.0f,
         0.1f,  1.0f,  1.0f, 1.0f
    };

    // -----------
    // Load models
    // -----------
    Model ourModel("res/models/river/river.obj");
    Model fishModel01("res/models/fish_01/fish.obj");

    // Position of point lights
    // ------------------------
    glm::vec3 pointLightPositions[] = 
    {
        glm::vec3( 0.7f,  5.2f,  0.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    // Configure the light cube's VAO and VBO
    // --------------------------------------
    unsigned int lightCubeVAO, VBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(lightCubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Configure the skybox VAO and VBO
    // --------------------------------
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glBindVertexArray(skyboxVAO);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Configure the water VAO and VBO
    // -------------------------------
    unsigned int waterVAO, waterVBO;
    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);

    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), &waterVertices, GL_STATIC_DRAW);
    glBindVertexArray(waterVAO);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Configure the water reflection VAO and VBO
    // ----------------------------------------
    unsigned int reflectionVAO, reflectionVBO;
    glGenVertexArrays(1, &reflectionVAO);
    glGenBuffers(1, &reflectionVBO);

    glBindBuffer(GL_ARRAY_BUFFER, reflectionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(reflectionVertices), &reflectionVertices, GL_STATIC_DRAW);

    glBindVertexArray(reflectionVAO);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Configure the water refraction VAO and VBO
    // ----------------------------------------
    unsigned int refractionVAO, refractionVBO;
    glGenVertexArrays(1, &refractionVAO);
    glGenBuffers(1, &refractionVBO);

    glBindBuffer(GL_ARRAY_BUFFER, refractionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(refractionVertices), &refractionVertices, GL_STATIC_DRAW);

    glBindVertexArray(refractionVAO);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // -------------
    // Load Textures
    // -------------
    std::vector<std::string> faces
    {
        "res/skybox/right.jpg",
        "res/skybox/left.jpg",
        "res/skybox/top.jpg",
        "res/skybox/bottom.jpg",
        "res/skybox/front.jpg",
        "res/skybox/back.jpg",
    };
    stbi_set_flip_vertically_on_load(false);
    unsigned int cubemapTexture = loadCubemap(faces);
    stbi_set_flip_vertically_on_load(true);

    // --------------------
    // Shader Configuration
    // --------------------
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    waterShader.use();
    waterShader.setInt("reflectionTexture", 0);

    // -------------------------
    // Framebuffer Configuration
    // -------------------------

    // Reflection framebuffer
    // ----------------------
    unsigned int reflectionFramebuffer;

    glGenFramebuffers(1, &reflectionFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebuffer);

    // Create a Color Attachment Texture
    unsigned int reflectionTextureColorbuffer;
    glGenTextures(1, &reflectionTextureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, reflectionTextureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTextureColorbuffer, 0);

    // Create a renderbuffer object for depth and stencil attachment
    unsigned int reflectionRbo;
    glGenRenderbuffers(1, &reflectionRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionRbo);

    // Use a single renderbuffer object for both a depth and stencil buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);

    // Attach the renderbuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, reflectionRbo);

    // Check and verify framebuffer status
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Refraction framebuffer
    // ----------------------
    unsigned int refractionFramebuffer;

    glGenFramebuffers(1, &refractionFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFramebuffer);

    // Create a Color Attachment Texture
    unsigned int refractionTextureColorbuffer;
    glGenTextures(1, &refractionTextureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, refractionTextureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTextureColorbuffer, 0);

    // Create a renderbuffer object for depth and stencil attachment
    unsigned int refractionRbo;
    glGenRenderbuffers(1, &refractionRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, refractionRbo);

    // Use a single renderbuffer object for both a depth and stencil buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);

    // Attach the renderbuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, refractionRbo);

    // Check and verify framebuffer status
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // -----------
    // Render Loop
    // -----------
    while(!glfwWindowShouldClose(window))       // Stops when window has been instructed to close
    {
        // Per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        // -------------
        processInput(window);

        // Enable Clipping
        // ---------------
        glEnable(GL_CLIP_DISTANCE0);

        // ------------------------------------
        // First Render Pass : Render As Normal
        // ------------------------------------

        // Bind back to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Also clear the depth buffer now

         // Activate shaders
        ourShader.use();
        ourShader.setVec4("plane", glm::vec4(0, 0, 0, 0));
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        /*
            Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
            the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
            by defining light types as classes and set their values in there, or by using a more efficient uniform approach
            by using "Uniform Buffer Objects", but that is something we'll discuss in the "Advanced GLSL" tutorial.
        */

        // Directional Light
        float sunDir = sin(glfwGetTime()) * 2.0f;
        // float sunDir = -0.2f;
        ourShader.setVec3("dirLight.direction", sunDir, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("dirLight.diffuse", 20.0f, 20.0f, 20.0f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // Point Light
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09f);
        ourShader.setFloat("pointLights[0].quadratic", 0.032f);

        // Spotlights
        if(spotlightToggle)
        {
            ourShader.setVec3("spotLight.position", camera.Position);
            ourShader.setVec3("spotLight.direction", camera.Front);
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 10.0f, 10.0f, 10.0f);
            ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            ourShader.setFloat("spotLight.constant", 1.0f);
            ourShader.setFloat("spotLight.linear", 0.09f);
            ourShader.setFloat("spotLight.quadratic", 0.032f);
            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        }
        else
        {
            ourShader.setVec3("spotLight.position", camera.Position);
            ourShader.setVec3("spotLight.direction", camera.Front);
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
            ourShader.setFloat("spotLight.constant", 1.0f);
            ourShader.setFloat("spotLight.linear", 0.09f);
            ourShader.setFloat("spotLight.quadratic", 0.032f);
            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        }
        
        // Transformations
        // ---------------

        // View / Projection Matrices

        // Reverse camera
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        // World transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);

        ourModel.Draw(ourShader);

        // Fish
        // ----

        // Reducing light intensities
        ourShader.setVec3("dirLight.diffuse", 0.0f, 0.0f, 0.0f);

        if(spotlightToggle)
            ourShader.setVec3("spotLight.diffuse", 0.05f, 0.05f, 0.05f);
        else
            ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);

        // World transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f, 0.7f, 1.0f));
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);

        fishModel01.Draw(ourShader);

        // Rendering the lamp object
        // -------------------------

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        // Draw light bulb
        glBindVertexArray(lightCubeVAO);
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[0]);
        model = glm::scale(model, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // -----
        // Water
        // -----
        waterShader.use();
        glBindVertexArray(reflectionVAO);

        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(12.5f, 1.0f, 11.0f));
        model = glm::translate(model, glm::vec3(0.5f, 1.0f, -0.7f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        waterShader.setMat4("model", model);
        waterShader.setMat4("view", view);
        waterShader.setMat4("projection", projection);
        // waterShader.setInt("refractionTexture", refractionTextureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, reflectionTextureColorbuffer);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw skybox
        // -----------
        glDepthFunc(GL_LEQUAL);     // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));    // Removes translation from view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // Skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);       // Set depth function back to default

        // ---------------------------------------------
        // Second Render Pass : Water Reflection Texture
        // ---------------------------------------------

        // Bind to framebuffer and draw scene as we normally would to color texture
        // ------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebuffer);
        glEnable(GL_DEPTH_TEST);        // Enable depth testing (It's disabled for rendering screen-space quad)

        // Render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Also clear the depth buffer now

         // Activate shaders
        ourShader.use();
        ourShader.setVec4("plane", glm::vec4(0, 1, 0, -1));
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        /*
            Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
            the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
            by defining light types as classes and set their values in there, or by using a more efficient uniform approach
            by using "Uniform Buffer Objects", but that is something we'll discuss in the "Advanced GLSL" tutorial.
        */

        // Directional Light
        // float sunDir = sin(glfwGetTime()) * 2.0f;
        // sunDir = -0.2f;
        ourShader.setVec3("dirLight.direction", sunDir, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("dirLight.diffuse", 20.0f, 20.0f, 20.0f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // Point Light
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09f);
        ourShader.setFloat("pointLights[0].quadratic", 0.032f);

        // Spotlights
        if(spotlightToggle)
        {
            ourShader.setVec3("spotLight.position", camera.Position);
            ourShader.setVec3("spotLight.direction", camera.Front);
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 10.0f, 10.0f, 10.0f);
            ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            ourShader.setFloat("spotLight.constant", 1.0f);
            ourShader.setFloat("spotLight.linear", 0.09f);
            ourShader.setFloat("spotLight.quadratic", 0.032f);
            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        }
        else
        {
            ourShader.setVec3("spotLight.position", camera.Position);
            ourShader.setVec3("spotLight.direction", camera.Front);
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
            ourShader.setFloat("spotLight.constant", 1.0f);
            ourShader.setFloat("spotLight.linear", 0.09f);
            ourShader.setFloat("spotLight.quadratic", 0.032f);
            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        }

        // Transformations
        // ---------------

        // View / Projection Matrices
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        // World transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);

        ourModel.Draw(ourShader);

        // Fish
        // ----

        // Reducing light intensities
        ourShader.setVec3("dirLight.diffuse", 0.0f, 0.0f, 0.0f);

        if(spotlightToggle)
            ourShader.setVec3("spotLight.diffuse", 0.05f, 0.05f, 0.05f);
        else
            ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);

        // World transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f, 0.7f, 1.0f));
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);

        fishModel01.Draw(ourShader);

        // Rendering the lamp object
        // -------------------------

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        // Draw light bulb
        glBindVertexArray(lightCubeVAO);
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[0]);
        model = glm::scale(model, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // // Water
        // // -----
        // waterShader.use();
        // glBindVertexArray(waterVAO);

        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(5.0f, 1.0f, 3.3f));
        // lightCubeShader.setMat4("model", model);
        // waterShader.setMat4("view", view);
        // waterShader.setMat4("projection", projection);
        // glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw skybox
        // -----------
        glDepthFunc(GL_LEQUAL);     // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));    // Removes translation from view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // Skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);       // Set depth function back to default

        // Framebuffer and Quads
        // ---------------------

        // Disable depth test so screen-space quad isn't discarded due to depth test
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        screenShader.use();
        glBindVertexArray(reflectionVAO);
        screenShader.setInt("screenTexture", 0);
        glBindTexture(GL_TEXTURE_2D, reflectionTextureColorbuffer);       // Use the color attachment texture as texture of quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);

        // --------------------------------------------
        // Third Render Pass : Water Refraction Texture
        // --------------------------------------------

        // Bind to framebuffer and draw scene as we normally would to color texture
        // ------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, refractionFramebuffer);
        glEnable(GL_DEPTH_TEST);        // Enable depth testing (It's disabled for rendering screen-space quad)

        // Render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Also clear the depth buffer now

         // Activate shaders
        ourShader.use();
        ourShader.setVec4("plane", glm::vec4(0, -1, 0, 1));
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        /*
            Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
            the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
            by defining light types as classes and set their values in there, or by using a more efficient uniform approach
            by using "Uniform Buffer Objects", but that is something we'll discuss in the "Advanced GLSL" tutorial.
        */

        // Directional Light
        // float sunDir = sin(glfwGetTime()) * 2.0f;
        sunDir = -0.2f;
        ourShader.setVec3("dirLight.direction", sunDir, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("dirLight.diffuse", 20.0f, 20.0f, 20.0f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // Point Light
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09f);
        ourShader.setFloat("pointLights[0].quadratic", 0.032f);

        // Spotlights
        if(spotlightToggle)
        {
            ourShader.setVec3("spotLight.position", camera.Position);
            ourShader.setVec3("spotLight.direction", camera.Front);
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 10.0f, 10.0f, 10.0f);
            ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            ourShader.setFloat("spotLight.constant", 1.0f);
            ourShader.setFloat("spotLight.linear", 0.09f);
            ourShader.setFloat("spotLight.quadratic", 0.032f);
            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        }
        else
        {
            ourShader.setVec3("spotLight.position", camera.Position);
            ourShader.setVec3("spotLight.direction", camera.Front);
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
            ourShader.setFloat("spotLight.constant", 1.0f);
            ourShader.setFloat("spotLight.linear", 0.09f);
            ourShader.setFloat("spotLight.quadratic", 0.032f);
            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        }

        // Transformations
        // ---------------

        // View / Projection Matrices
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        // World transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);

        ourModel.Draw(ourShader);

        // Fish
        // ----

        // Reducing light intensities
        ourShader.setVec3("dirLight.diffuse", 0.0f, 0.0f, 0.0f);

        if(spotlightToggle)
            ourShader.setVec3("spotLight.diffuse", 0.05f, 0.05f, 0.05f);
        else
            ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);

        // World transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f, 0.7f, 1.0f));
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);

        fishModel01.Draw(ourShader);

        // Rendering the lamp object
        // -------------------------

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        // Draw light bulb
        glBindVertexArray(lightCubeVAO);
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[0]);
        model = glm::scale(model, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // // Water
        // // -----
        // waterShader.use();
        // glBindVertexArray(waterVAO);

        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(5.0f, 1.0f, 3.3f));
        // lightCubeShader.setMat4("model", model);
        // waterShader.setMat4("view", view);
        // waterShader.setMat4("projection", projection);
        // glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw skybox
        // -----------
        glDepthFunc(GL_LEQUAL);     // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));    // Removes translation from view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // Skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);       // Set depth function back to default

        // Framebuffer and Quads
        // ---------------------

        // Disable depth test so screen-space quad isn't discarded due to depth test
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        screenShader.use();
        glBindVertexArray(refractionVAO);
        glBindTexture(GL_TEXTURE_2D, refractionTextureColorbuffer);       // Use the color attachment texture as texture of quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);

        // GLFW : swap buffers and poll IO events (keys pressed/released, mouse moved etc)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // GLFW : Terminate, clearing all previously allocated GLFW resources
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// GLFW : Whenever the window size is changed (by OS or user resize), this callback function executes
// --------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
}

// Processing all input : query GLFW whether relevant keys are pressed/released this frame and react accordingly
// -------------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // WASD Movement
    // -------------
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    // EQ Camera Up/Down Movement
    // --------------------------
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    // Spotlight toggle
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
        spotlightToggleReleased = true;
    else if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && spotlightToggleReleased == true)
    {
        spotlightToggle ^= 0x1;
        spotlightToggleReleased = false;
    }
}

// GLFW : Whenever the mouse moves, this callback function is called
// -----------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;       // Reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

// GLFW : Whenever the mouse scroll wheel scrolls, this callback is called
// -----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}

// Utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if(data)
    {
        GLenum format;
        if(nrComponents == 1)
            format = GL_RED;
        else if(nrComponents == 3)
            format = GL_RGB;
        else if(nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to laod at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// Loads a cubemap texture from 6 individual texture faces
// Order :
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, numComponents;

    for(unsigned int i = 0; i < faces.size(); ++i)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &numComponents, 0);

        if(data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path : " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return textureID;
}