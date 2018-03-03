#ifndef SHADER_H
#define SHADER_H

#include "glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/**
    ------------- SHADER PROGRAM -------------
    The graphics pipeline can be divided into two large parts: the first transforms your 3D coordinates into 2D coordinates 
    and the second part transforms the 2D coordinates into actual colored pixels.
    The geometry shader takes as input a collection of vertices that form a primitive and has the ability 
    to generate other shapes by emitting new vertices to form new (or other) primitive(s).
*/

class Shader {
public:
    unsigned int shaderProgram;

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char * pathVertexShader, const char * pathFragmentShader) {

        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try {
            // open files
            vShaderFile.open(pathVertexShader);
            fShaderFile.open(pathFragmentShader);
            std::stringstream vShaderStream, fShaderStream;

            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            // close file handlers
            vShaderFile.close();
            fShaderFile.close();

            // convert stream into string
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();

        // 2. compile shaders
        unsigned int vertexShader, fragmentShader;
        int success;
        char infoLog[512];


        // Vertex Shader: takes as input a single vertex and return normalized device coordinates
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, NULL); // to use the shader it has to dynamically compile it at run-time from its source code
        glCompileShader(vertexShader);
        checkCompileErrors(vertexShader, "VERTEX");  // check for shader compile errors (vertex)


        // Fragment Shader : is all about calculating the color output of your pixels
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL); 
        glCompileShader(fragmentShader);
        checkCompileErrors(fragmentShader, "FRAGMENT"); // check for shader compile errors (fragment)


        /**
            Shader Program that we can use for rendering
            To use the recently compiled shaders we have to link them to a shader program object 
            and then activate this shader program when rendering objects.
        */
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        checkCompileErrors(shaderProgram, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() { 
        glUseProgram(shaderProgram); 
    }

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const {         
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value); 
    }

    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const { 
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value); 
    }
    
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const { 
        glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value); 
    }

    // ------------------------------------------------------------------------

    void setVec2(const std::string &name, const glm::vec2 &value) const { 
        glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]); 
    }

    void setVec2(const std::string &name, float x, float y) const { 
        glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y); 
    }

    // ------------------------------------------------------------------------

    void setVec3(const std::string &name, const glm::vec3 &value) const { 
        glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]); 
    }

    void setVec3(const std::string &name, float x, float y, float z) const { 
        glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z); 
    }

    // ------------------------------------------------------------------------

    void setVec4(const std::string &name, const glm::vec4 &value) const { 
        glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]); 
    }

    void setVec4(const std::string &name, float x, float y, float z, float w) const { 
        glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w); 
    }

    // ------------------------------------------------------------------------

    void setMat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    // ------------------------------------------------------------------------

    void setMat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    // ------------------------------------------------------------------------
    
    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    /**
        check for linking errors (program)
        When linking the shaders into a program it links the outputs of each shader to the inputs of the next shader. 
        This is also where you'll get linking errors if your outputs and inputs do not match.
    */    
    void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif