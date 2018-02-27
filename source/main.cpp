/**
    ref: https://learnopengl.com
    glfw3: brew install glfw3
    g++ -c main.cpp && gcc -c glad.c && g++ -lglfw glad.o main.o -o main
    Costanza Volpini
*/

#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

/**
    Modern OpenGL requires that we at least set up a vertex and fragment shader if we want to do some rendering.
    Shader language GLSL (OpenGL Shading Language)
    Each shader begins with a declaration of its version. 
    Since OpenGL 3.3 and higher the version numbers of GLSL match the version of OpenGL 
    (GLSL version 420 corresponds to OpenGL version 4.2 for example).
 */


/** 
    ------------- VERTEX SHADER -------------
    create a vec3 called aPos
    cast this to a vector of size 4
*/
const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aCoords;\n"
    "out vec3 Coords;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   Coords = aCoords;\n"
    "}\0";


/** 
    ------------- FRAGMENT SHADER -------------
    format RGBA
    where alpha value is a value from 0.0 to 1.0 (1.0 being completely opaque).
*/ 
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 Coords;\n"
    "void main()\n"
    "{\n"
        "if (Coords.x < 0.333)\n"
        "FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
        "else\n"
        "FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\n\0";

int main() {
    /**
        ------------- GLFW -------------
        initialize glf library
     */ 
    glfwInit();

    // configure GLFW - OpenGl version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for OS X
    #endif

    /**
        create a window object
        glfwCreateWindow (int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share)
    */
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bachelor Project", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // user resizes the window the viewport should be adjusted as well
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /**
        ------------- GLAD -------------
        GLAD manages function pointers for OpenGL so we want to initialize GLAD before we call any OpenGL function
    */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }    

    // ------------- END GLAD -------------

    /**
        ------------- SHADER PROGRAM -------------
        The graphics pipeline can be divided into two large parts: the first transforms your 3D coordinates into 2D coordinates 
        and the second part transforms the 2D coordinates into actual colored pixels.
        The geometry shader takes as input a collection of vertices that form a primitive and has the ability 
        to generate other shapes by emitting new vertices to form new (or other) primitive(s).
    */

    // Vertex Shader: takes as input a single vertex and return normalized device coordinates
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // to use the shader it has to dynamically compile it at run-time from its source code
    glCompileShader(vertexShader); 

    // check for shader compile errors (vertex)
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // Fragment Shader : is all about calculating the color output of your pixels
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors (fragment)
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    /**
        Shader Program that we can use for rendering
        To use the recently compiled shaders we have to link them to a shader program object 
        and then activate this shader program when rendering objects.
    */
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    /**
        check for linking errors (program)
        When linking the shaders into a program it links the outputs of each shader to the inputs of the next shader. 
        This is also where you'll get linking errors if your outputs and inputs do not match.
    */
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /**
        ------------- VERTEX DATA -------------
        input: Vertex Data. This vertex data is a collection of vertices.
        normalized device coordinates range will end up visible on your screen (and all coordinates outside this region won't)
        from -1.0 to 1.0
    */

    float vertices[] = {
        // positions         // coordinates
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };   

    /**
        NB. OpenGL works in 3D space we render a 2D triangle with each vertex having a z coordinate of 0.0.
        This way the depth of the triangle remains the same making it look like it's 2D.
        
        Send vertex data to vertex shader. 
        In this part we tell to OpenGl how it should interpret the vertex data in memory and how it should connect the vertex data to the vertex shader's attributes.
     */ 

    /**
        Create memory on the GPU where we store the vertex data, configure how OpenGL should interpret the memory and 
        specify how to send the data to the graphics card.
        VBO: manage this memory via so called vertex buffer objects (VBO) that can store a large number of vertices in the GPU's memory
    */
    unsigned int VBO, VAO;
    /**
        ------------- VBO -------------
        advantage of using those buffer objects is that we can send large batches of data all at once 
        to the graphics card without having to send data a vertex a time
    */
    glGenBuffers(1, &VBO); //generate buffer, bufferID = 1

    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    /**
        buffer type of a vertex buffer object is GL_ARRAY_BUFFER
        From that point (bind) on any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to configure the currently bound buffer, which is VBO.
        NB for glBufferData: the fourth parameter specifies how we want the graphics card to manage the given data.
    */

    /**
        GL_STATIC_DRAW: the data will most likely not change at all or very rarely.
        GL_DYNAMIC_DRAW: the data is likely to change a lot.
        GL_STREAM_DRAW: the data will change every time it is drawn.
    */
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copies the previously defined vertex data into the buffer's memor


    // ------------- VAO -------------
    glGenVertexArrays(1, &VAO);

    /**
        bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        VAO: advantage that when configuring vertex attribute pointers you only have to make those calls once and 
        whenever we want to draw the object, we can just bind the corresponding VAO
    */
    glBindVertexArray(VAO);
    // If we fail to bind a VAO, OpenGL will most likely refuse to draw anything.

    /**
        void glVertexAttribPointer(GLuint index​, GLint size​, GLenum type​, GLboolean normalized​, GLsizei stride​, const GLvoid * pointer​);
        NB we specified the location of the position vertex attribute in the vertex shader with layout (example: location = 0). We want to pass data to this vertex attribute, we pass in 0 (since location = 0).
        size = 6 since we want to pass 3 values (it is a vec3) and a colour for each vertex.
        stride and tells us the space between consecutive vertex attribute sets
        offset of where the position data begins in the buffer. Since the position data is at the start of the data array this value is just 0. 
    */
    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // 72-bit floating point values, each position is composed of 6 of those values (3 points + 3 colours (one for each vertex))
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);


    /**
        You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    */
    glBindVertexArray(0); 


    /**
        application to keep drawing images and handling user input until the program has been explicitly told to stop
        render loop
    */
    while(!glfwWindowShouldClose(window)) { // function checks at the start of each loop iteration if GLFW has been instructed to close

        // process inputs whether relevant keys are pressed/released this frame and react accordingly
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // render colours
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black screen
        glClear(GL_COLOR_BUFFER_BIT);

        // draw
        glUseProgram(shaderProgram);

        /**
            Every shader and rendering call after glUseProgram will now use this program object (and thus the shaders).
            The output of the geometry shader is then passed on to the rasterization stage where it maps the resulting primitive(s) 
            to the corresponding pixels on the final screen, resulting in fragments for the fragment shader to use.  
            + Clipping (discards all fragments that are outside your view, increasing performance).
        */

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window); // will swap the color buffer
        glfwPollEvents(); // function checks if any events are triggered (like keyboard input or mouse movement events) 
    }

    // delete the shader objects once we've linked them into the program object; we no longer need them anymore
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // clean/delete all resources that were allocated
    glfwTerminate(); 
    return 0;
}


// whenever the window size changed this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    /**
        make sure the viewport matches the new window dimensions; note that width and 
        height will be significantly larger than specified on retina displays.
    */
    glViewport(0, 0, width, height);
}

