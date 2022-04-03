#ifndef VAOVBO_LOADER_H
#define VAOVBO_LOADER_H

#include <glad/glad.h>

#include <iostream>

class VaoVboLoader
{
public:
    // Load light cube VAO and VBO
    static void loadLightCube(unsigned int &lightCubeVAO, unsigned int &VBO, float* vertices)
    {
        glGenVertexArrays(1, &lightCubeVAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 144 * sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(lightCubeVAO);

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    // Load skybox VAO and VBO
    static void loadSkybox(unsigned int &skyboxVAO, unsigned int skyboxVBO, float* vertices)
    {
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);

        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, 54 * sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(skyboxVAO);

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
    }

    // Load water surface VAO and VBO
    static void loadWater(unsigned int &waterVAO, unsigned int waterVBO, float* vertices)
    {
        glGenVertexArrays(1, &waterVAO);
        glGenBuffers(1, &waterVBO);

        glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
        glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(waterVAO);

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // Load water reflection and refraction VAO and VBO
    static void loadReflectRefract(unsigned int &reflectRefractVAO, unsigned int reflectRefractVBO, float* vertices)
    {
        glGenVertexArrays(1, &reflectRefractVAO);
        glGenBuffers(1, &reflectRefractVBO);

        glBindBuffer(GL_ARRAY_BUFFER, reflectRefractVBO);
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(reflectRefractVAO);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    
    // Load framebuffer
    static void loadFramebuffer(unsigned int &framebuffer, unsigned int &colorBuffer, unsigned int &rbo)
    {
        glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create a Color Attachment Texture
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1200, 900, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    // Create a renderbuffer object for depth and stencil attachment
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    // Use a single renderbuffer object for both a depth and stencil buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1200, 900);

    // Attach the renderbuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check and verify framebuffer status
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

#endif