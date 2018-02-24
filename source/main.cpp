/**
    ref: https://learnopengl.com
    glfw3: brew install glfw3
    g++ -c main.cpp && gcc -c glad.c && g++ -lglfw glad.o main.o -o main
    Costanza Volpini
*/

// create an OpenGL context and an application window to draw in

#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main() {
    // ------------- GLFW -------------

    // initialize glf library
    glfwInit();

    // configure GLFW - OpenGl version (3 and 4)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for OS X
    #endif


    // create a window object
    // glfwCreateWindow (int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share)
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bachelor Project", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // user resizes the window the viewport should be adjusted as well
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ------------- GLAD -------------

    // GLAD manages function pointers for OpenGL so we want to initialize GLAD before we call any OpenGL function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }    

    // ------------- END GLAD -------------

    // application to keep drawing images and handling user input until the program has been explicitly told to stop
    // render loop
    while(!glfwWindowShouldClose(window)) { // function checks at the start of each loop iteration if GLFW has been instructed to close
        // process inputs whether relevant keys are pressed/released this frame and react accordingly
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // render colours
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glfwSwapBuffers(window); // will swap the color buffer
        glfwPollEvents(); // function checks if any events are triggered (like keyboard input or mouse movement events) 
    }

    // clean/delete all resources that were allocated
    glfwTerminate(); 
    return 0;
}


// whenever the window size changed this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}