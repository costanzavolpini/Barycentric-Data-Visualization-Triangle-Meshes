/**
    ref: https://learnopengl.com
    glfw3: brew install glfw3
    g++ -c main.cpp && gcc -c glad.c && g++ -lglfw glad.o main.o -o main 
    or just use make clean && make && ./main
    Costanza Volpini
*/

#include "Base.h"
#include "Shader.h"
#include "Arcball.h"
#include "Object.h"

//to test
#include "glm/ext.hpp"


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
    // glm::vec3 get_arcball_vector(double x, double y);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    

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
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


    // mouse 
    // glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

    // callback functions
    glfwSetScrollCallback(window, scroll_callback); //zoom
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // user resizes the window the viewport should be adjusted as well
    glfwSetMouseButtonCallback(window, mouse_button_callback); // call the callback when the user press a button. It corresponds to glutMouseFunc
    glfwSetCursorPosCallback(window, cursor_position_callback); // call the callback when the user move the cursor. It corresponds to glutMotionFunc

    // tell GLFW to capture our mouse
    // (GLFWwindow * window, int mode, int value)
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    Shader ourShader("vertexShader.vs", "maxDiagramFragmentShader.fs", "geometryShader.gs");

    /**
        NB. OpenGL works in 3D space we render a 2D triangle with each vertex having a z coordinate of 0.0.
        This way the depth of the triangle remains the same making it look like it's 2D.
        
        Send vertex data to vertex shader (load .off file). 
     */ 
    
    Object object = Object("models/iCorsi/icosahedron_0.off");
    object.init();

    /**
        IMPORTANT FOR TRANSFORMATION:
        Since GLM version 0.9.9, GLM default initializates matrix types to a 0-initalized matrix, 
        instead of the identity matrix. From that version it is required to initialize matrix types as: glm::mat4 mat = glm::mat4(1.0f). 
    */ 
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0., 0., 0.), glm::vec3(0., 1., 0.));
    glm::mat4 projection = glm::perspective(glm::radians(Zoom), (float)WIDTH / (float)HEIGHT, 3.0f, 10.0f); //near plane must be close of the the camera location (aound 3.0f)
    glm::mat4 model = glm::mat4(1.0f);

    ourShader.use(); //draw

        // get matrix's uniform location and set matrix
        ourShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f); 
        ourShader.setVec3("viewPos", glm::vec3(0.0f, 0.0f, 3.0f));

        // light properties
        ourShader.setVec3("light.ambient", 1.2f, 1.2f, 1.2f);
        ourShader.setVec3("light.diffuse", 1.5f, 1.5f, 1.5f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("shininess", 32.0f);
    /**
        application to keep drawing images and handling user input until the program has been explicitly told to stop
        render loop
    */
    while(!glfwWindowShouldClose(window)) { // function checks at the start of each loop iteration if GLFW has been instructed to close
        // keyboard
        process_input(window);

        // render colours
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the depth buffer before each render iteration (otherwise the depth information of the previous frame stays in the buffer).

        // arcball
        glm::mat4 rotated_view = view * arcball.rotation_matrix_view();
        glm::mat4 rotated_model = model * arcball.rotation_matrix_model(view);
        projection = glm::perspective(glm::radians(Zoom), (float)WIDTH / (float)HEIGHT, 3.0f, 10.0f);

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", rotated_view);
        ourShader.setMat4("model", rotated_model);

        object.draw();

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
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
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