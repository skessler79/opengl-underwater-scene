#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // The program ID
    unsigned int ID;

    // // Constructor generates the shader on the fly (vertex and fragment only)
    // // ----------------------------------------------------------------------
    // Shader(const char* vertexPath, const char* fragmentPath)
    // {
    //     // 1. Retrieve the vertex/fragment source code from filePath
    //     // ---------------------------------------------------------
    //     std::string vertexCode;
    //     std::string fragmentCode;
    //     std::ifstream vShaderFile;
    //     std::ifstream fShaderFile;

    //     // Ensure ifstream objects can throw exceptions
    //     vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    //     fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    //     try
    //     {
    //         // Open files
    //         vShaderFile.open(vertexPath);
    //         fShaderFile.open(fragmentPath);
    //         std::stringstream vShaderStream, fShaderStream;

    //         // Read file's buffer contents into streams
    //         vShaderStream << vShaderFile.rdbuf();
    //         fShaderStream << fShaderFile.rdbuf();

    //         // Close file handlers
    //         vShaderFile.close();
    //         fShaderFile.close();

    //         // Convert stream into string
    //         vertexCode = vShaderStream.str();
    //         fragmentCode = fShaderStream.str();
    //     }
    //     catch (std::ifstream::failure& e)
    //     {
    //         std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    //     }

    //     const char* vShaderCode = vertexCode.c_str();
    //     const char* fShaderCode = fragmentCode.c_str();

    //     // 2. Compile shaders
    //     // ------------------
    //     unsigned int vertex, fragment;

    //     // Vertex Shader
    //     vertex = glCreateShader(GL_VERTEX_SHADER);
    //     glShaderSource(vertex, 1, &vShaderCode, NULL);
    //     glCompileShader(vertex);
    //     checkCompileErrors(vertex, "VERTEX");

    //     // Fragment Shader
    //     fragment = glCreateShader(GL_FRAGMENT_SHADER);
    //     glShaderSource(fragment, 1, &fShaderCode, NULL);
    //     glCompileShader(fragment);
    //     checkCompileErrors(fragment, "FRAGMENT");

    //     // Shader Program
    //     ID = glCreateProgram();
    //     glAttachShader(ID, vertex);
    //     glAttachShader(ID, fragment);
    //     glLinkProgram(ID);
    //     checkCompileErrors(ID, "PROGRAM");

    //     // Delete the shaders as they're linked into our program now and no longer necessary
    //     glDeleteShader(vertex);
    //     glDeleteShader(fragment);
    // }

    // Constructor generates shader on the fly (with tessellation and geometry)
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr,
           const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr)
    {
        // 1. Retrieve the vertex / fragment source code from filePath
        // -----------------------------------------------------------
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::string tessControlCode;
        std::string tessEvalCode;

        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        std::ifstream tcShaderFile;
        std::ifstream teShaderFile;

        // Ensure ifstream objects can throw exceptions
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            // Close file handlers
            vShaderFile.close();
            fShaderFile.close();

            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();

            // If geometry shader path is present, load a geometry shader
            if(geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }

            // If tessellation control shader path is present, load a tessellation control shader
            if(tessControlPath != nullptr)
            {
                tcShaderFile.open(tessControlPath);
                std::stringstream tcShaderStream;
                tcShaderStream << tcShaderFile.rdbuf();
                tcShaderFile.close();
                tessControlCode = tcShaderStream.str();
            }

            // If tessellation evaluation shader path is present, load a tessellation evaluation shader
            if(tessEvalPath != nullptr)
            {
                teShaderFile.open(tessEvalPath);
                std::stringstream teShaderStream;
                teShaderStream << teShaderFile.rdbuf();
                teShaderFile.close();
                tessEvalCode = teShaderStream.str();
            }
        }
        catch(std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        // 2. Compile Shaders
        // ------------------

        unsigned int vertex, fragment;

        // Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        // Fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // If geometry shader is given, compile geometry shader
        unsigned int geometry;
        if(geometryPath != nullptr)
        {
            const char * gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }

        // If tessellation control shader is given, compile tessellation control shader
        unsigned int tessControl;
        if(tessControlPath != nullptr)
        {
            const char* tcShaderCode = tessControlCode.c_str();
            tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
            glShaderSource(tessControl, 1, &tcShaderCode, NULL);
            glCompileShader(tessControl);
            checkCompileErrors(tessControl, "TESS_CONTROL");
        }

        // If tessellation evaluation shader is given, compile tessellation evaluation shader
        unsigned int tessEval;
        if(tessEvalPath != nullptr)
        {
            const char* teShaderCode = tessEvalCode.c_str();
            tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
            glShaderSource(tessEval, 1, &teShaderCode, NULL);
            glCompileShader(tessEval);
            checkCompileErrors(tessEval, "TESS_EVALUATION");
        }

        // 3. Shader program
        // -----------------
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if(geometryPath != nullptr)
            glAttachShader(ID, geometry);
        if(tessControlPath != nullptr)
            glAttachShader(ID, tessControl);
        if(tessEvalPath != nullptr)
            glAttachShader(ID, tessEval);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        // Delete the shaders as they're linked into our program now and are not longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if(geometryPath != nullptr)
            glDeleteShader(geometry);

    }

    // Use/Activate the shader
    // -----------------------
    void use()
    {
        glUseProgram(ID);
    }

    // Utility uniform functions
    // -------------------------
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // Utility function for checking shader compilation/linking errors
    // ---------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << '\n' << infoLog
                          << "\n ----------------------------------------------------" << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << '\n' << infoLog
                          << "\n-----------------------------------------------------" << std::endl;
            }
        }
    }
};


#endif