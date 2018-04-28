/**
    ref: https://learnopengl.com
    glfw3: brew install glfw3
    glew: brew install glew
    just use make clean && make && ./main
    Costanza Volpini
*/

#include "Base.h"
#include "Shader.h"
#include "Arcball.h"
#include "Object.h"
#include "LoaderObject.h"
#include <math.h>
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

//to test
#include "glm/ext.hpp"

#define IS_IN_DEBUG false // to show normals

using namespace std;

    // settings
    const unsigned int WIDTH = 800;
    const unsigned int HEIGHT = 600;

    // Arcball instance
    static Arcball arcball(WIDTH, HEIGHT, 1.5f, true, true);

    void error_callback(int error, const char * desc);

    // Camera options
    float Zoom = 45.0f;

    // resize window
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    // keyboard
    void process_input(GLFWwindow *window);

    double last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;
    int arcball_on = false;

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


int main(int argc, char * argv[]) {  //arguments: nameFile type(example: gc is gaussian curvature, li is linearly interpolated, efs is extension of flat shading)
    string name_file = "models/iCorsi/icosahedron_1.off"; //default name
    string type = "efs"; //default type
    const char * vertex_shader = "vertexShader.vs";
    const char * geometry_shader = "geometryShader.gs";
    const char * fragment_shader = "maxDiagramFragmentShader.fs";
    int isGaussianCurvature = 0;
    int isLinearInterpolation = 0;
    int isExtendFlatShading = 1;
    int isGouraudShading = 1;

    /*
       Take input
     */
    if (argc > 1) {
        char * token;
        for (int elem = 1; elem < argc; elem++) {
            token = strtok(argv[elem], "=");
            if (strcmp(token, "name") == 0) {
                name_file = strtok(NULL, "=");
            } else if (strcmp(token, "type") == 0) {
                type = strtok(NULL, "=");
                if(type == "gc"){ // GAUSSIAN CURVATURE
                    setGaussianCurvature(1); //pass to LoaderObject
                    setExtendFlatShading(0); //pass to LoaderObject
                    setGouraudShading(0); //pass to LoaderObject
                    vertex_shader = "vertexShaderGC.vs";
                    isGaussianCurvature = 1;
                    isExtendFlatShading = 0;
                    isGouraudShading = 0;
                } else if(type == "li"){ // LINEAR INTERPOLATION
                    setLinearInterpolation(1); //pass to LoaderObject
                    setExtendFlatShading(0); //pass to LoaderObject
                    setGouraudShading(0); //pass to LoaderObject
                    vertex_shader = "vertexShaderLI.vs";
                    fragment_shader = "fragmentShader.fs";
                    geometry_shader = NULL;
                    isLinearInterpolation = 1;
                    isExtendFlatShading = 0;
                    isGouraudShading = 0;
                } else if(type == "gs"){
                    setLinearInterpolation(0); //pass to LoaderObject
                    setExtendFlatShading(0); //pass to LoaderObject
                    setGouraudShading(1); //pass to LoaderObject
                    vertex_shader = "vertexShader.vs";
                    fragment_shader = "fragmentShader.fs";
                    geometry_shader = NULL;
                    isGouraudShading = 1;
                    isLinearInterpolation = 0;
                    isExtendFlatShading = 0;
                } else if(type == "gcli"){ //gaussian curvature linear interpolation
                    setLinearInterpolation(0); //pass to LoaderObject
                    setExtendFlatShading(0); //pass to LoaderObject
                    setGouraudShading(0); //pass to LoaderObject
                    setGaussianCurvature(1);
                    vertex_shader = "vertexShaderGC.vs";
                    fragment_shader = "fragmentShader.fs";
                    geometry_shader = NULL;
                    isGouraudShading = 0;
                    isLinearInterpolation = 0;
                    isExtendFlatShading = 0;
                    isGaussianCurvature = 1;
                } // else EXTEND FLAT SHADING (ef)
            }
        }
    }


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
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


    // callback functions
    glfwSetScrollCallback(window, scroll_callback); //zoom
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // user resizes the window the viewport should be adjusted as well
    glfwSetMouseButtonCallback(window, mouse_button_callback); // call the callback when the user press a button. It corresponds to glutMouseFunc
    glfwSetCursorPosCallback(window, cursor_position_callback); // call the callback when the user move the cursor. It corresponds to glutMotionFunc

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
        Modern OpenGL requires that we at least set up a vertex and fragment shader if we want to do some rendering.
        Shader language GLSL (OpenGL Shading Language)
        Each shader begins with a declaration of its version.
        Since OpenGL 3.3 and higher the version numbers of GLSL match the version of OpenGL
        (GLSL version 420 corresponds to OpenGL version 4.2 for example).
    */
    Shader ourShader(vertex_shader, fragment_shader, geometry_shader);

    Shader normalShader("normal.vs", "normal.fs", "normal.gs");



    /**
        NB. OpenGL works in 3D space we render a 2D triangle with each vertex having a z coordinate of 0.0.
        This way the depth of the triangle remains the same making it look like it's 2D.

        Send vertex data to vertex shader (load .off file).
     */
    Object object = Object(name_file);
    object.setGaussianCurvature(isGaussianCurvature);
    object.setExtendFlatShading(isExtendFlatShading);
    object.setGouraudFlatShading(isGouraudShading);
    object.setLinearInterpolation(isLinearInterpolation);
    object.init();

    /**
        IMPORTANT FOR TRANSFORMATION:
        Since GLM version 0.9.9, GLM default initializates matrix types to a 0-initalized matrix,
        instead of the identity matrix. From that version it is required to initialize matrix types as: glm::mat4 mat = glm::mat4(1.0f).
    */
    // camera position (eye) - look at origin - head is up
    glm::mat4 view = glm::lookAt(glm::vec3(4.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 model = glm::mat4(1.0f);

    ourShader.use(); //draw


    if(isExtendFlatShading || isGouraudShading){
        // get matrix's uniform location and set matrix
        ourShader.setVec3("light.position", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("viewPos", glm::vec3(0.0f, 0.0f, 3.0f));

        // light properties
        ourShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("light.specular", 0.2f, 0.2f, 0.2f);
        ourShader.setFloat("shininess", 12.0f);

    } else if(isGaussianCurvature) {
        // gaussian curvature
        ourShader.setFloat("min_gc", object.get_minimum_gaussian_curvature_value());
        ourShader.setFloat("max_gc", object.get_maximum_gaussian_curvature_value());
        ourShader.setFloat("mean_negative_gc", object.get_negative_mean_gaussian_curvature_value());
        ourShader.setFloat("mean_positive_gc", object.get_positive_mean_gaussian_curvature_value());
        // cout << "MIN " << object.get_minimum_gaussian_curvature_value() << endl;
        // cout << "MAX " << object.get_maximum_gaussian_curvature_value() << endl;
    }

    /**
        application to keep drawing images and handling user input until the program has been explicitly told to stop
        render loop
    */
    while(!glfwWindowShouldClose(window)) { // function checks at the start of each loop iteration if GLFW has been instructed to close
        // keyboard
        process_input(window);

        glEnable(GL_DEPTH_TEST);

        // render colours
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the depth buffer before each render iteration (otherwise the depth information of the previous frame stays in the buffer).

        glm::mat4 projection = glm::perspective(glm::radians(Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 10.f);

        // arcball
        glm::mat4 rotated_view = view * arcball.rotation_matrix_view();
        glm::mat4 rotated_model = model * arcball.rotation_matrix_model(rotated_view);

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", rotated_view);
        ourShader.setMat4("model", rotated_model);

        glm::mat4 transform = glm::mat4(1.0f);
        // transform = model * glm::rotate(transform, 90.0f, glm::vec3(1.0f, 0.0f, 1.0f));
        // transform = model * glm::translate(transform, glm::vec3(0.0f, 0.0f, -5.0f));
        // transform = model * glm::rotate(transform, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 1.0f));
        // ourShader.setMat4("model", transform);
        object.draw();

        if (IS_IN_DEBUG){
            // then draw model with normal visualizing geometry shader (FOR DEBUG)
            normalShader.use();
            normalShader.setMat4("projection", projection);
            normalShader.setMat4("view", rotated_view);
            normalShader.setMat4("model", rotated_model);

            object.draw();
        }

        glfwSwapBuffers(window); // will swap the color buffer
        glfwPollEvents(); // function checks if any events are triggered (like keyboard input or mouse movement events)
    }

    ourShader.deactivate();

    // delete the shader objects once we've linked them into the program object; we no longer need them anymore
    object.clear();

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

// Function that activate arcball when button left is pressed.
// mouse button, button action and modifier bits.
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
     arcball.mouse_btn_callback(window, button, action, mods);
}

// Function that take position when arcball is activate (left button is pressed)
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
  arcball.cursor_position_callback( window, xpos, ypos );
}


// keyboard
void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
}

