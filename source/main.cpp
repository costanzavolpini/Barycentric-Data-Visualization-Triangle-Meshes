/**
    ref: https://learnopengl.com
    glfw3: brew install glfw3
    g++ -c main.cpp && gcc -c glad.c && g++ -lglfw glad.o main.o -o main 
    or just use make clean && make && ./main
    Costanza Volpini
*/

#include "Base.h"
#include "Light.h"
#include "Color.h"
#include "Shader.h"
#include "Camera.h"
#include "Object.h"

using namespace std;

// settings
    const unsigned int WIDTH = 800;
    const unsigned int HEIGHT = 600;

    // animation parameter (time)
    double tau;

    // List of pointers to all the lights
    vector<Light *> lights;

	// List of pointers to all the objects
	// vector<Object *> objects;

    // Camera settings
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    float lastX = WIDTH / 2.0f;
    float lastY = HEIGHT / 2.0f;
    bool firstMouse = true;

    // functions used for ray tracing
	// Color3d trace(Ray Ray, int cnt);
    // // Object * findNearestObject(Ray ray);
	// Color3d PhongLighting(Point3d p, Point3d n, Point3d v, Material mat);
	// bool inShadow(Point3d p, Light * l);

    // Timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

        // resize window
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    // function for mouse
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    // keyboard
    void processInput(GLFWwindow *window);

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

    // mouse 
    glfwSetCursorPosCallback(window, mouse_callback); // x pos
    glfwSetScrollCallback(window, scroll_callback); //y pos

    // tell GLFW to capture our mouse
    // (GLFWwindow * window, int mode, int value)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    Shader ourShader("vertexShader.vs", "maxDiagramFragmentShader.fs");

    /**
        NB. OpenGL works in 3D space we render a 2D triangle with each vertex having a z coordinate of 0.0.
        This way the depth of the triangle remains the same making it look like it's 2D.
        
        Send vertex data to vertex shader (load .off file). 
     */ 
    
    Object object = Object("models/iCorsi/icosahedron_0.off");
    object.init();

    /**
        Light source (white light from top left)
    */ 
    // Material LightCol;
    //     LightCol.ambient = Color3d(0.6, 0.6, 0.6);
    //     LightCol.diffuse = Color3d(1.0, 1.0, 1.0);
    //     LightCol.specular = Color3d(1.0, 1.0, 1.0);

    // Point3d LightPos(-10.0, 5.0, -5.0);

    // Light* light = new Light(LightPos, LightCol);
    // light->setDirectional();

    // lights.push_back(light);

    /**
        application to keep drawing images and handling user input until the program has been explicitly told to stop
        render loop
    */
    while(!glfwWindowShouldClose(window)) { // function checks at the start of each loop iteration if GLFW has been instructed to close
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // keyboard
        processInput(window);


        // render colours
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black screen
        glClear(GL_COLOR_BUFFER_BIT);

        // get matrix's uniform location and set matrix
        ourShader.use(); //draw


        /**
            IMPORTANT FOR TRANSFORMATION:
            Since GLM version 0.9.9, GLM default initializates matrix types to a 0-initalized matrix, 
            instead of the identity matrix. From that version it is required to initialize matrix types as: glm::mat4 mat = glm::mat4(1.0f). 
        */ 
        //frustum
        //glm::perspective = field of view (zoom), aspect (height of frustum), near plane, far plane
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        //view
        glm::mat4 view = glm::mat4(1.0f);
        // The glm::LookAt function requires a camera position, target/position the camera should look at and up vector that represents the up vector in world space.
        // camera.GetViewMatrix call a LookAt with: (eyeX, eyeY, eyeZ) (centerX, centerY, centerZ) (upX, upY, upZ)
        view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // create transformations
        glm::mat4 trans = glm::mat4(1.0f);
        // trans = glm::translate(trans, glm::vec3(-px, -py, -pz));
        // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        unsigned int transformLoc = glGetUniformLocation(ourShader.shaderProgram, "transform");
        /**
            The first argument should be familiar by now which is the uniform's location. 
            The second argument tells OpenGL how many matrices we'd like to send, which is 1. 
            The third argument asks us if we want to transpose our matrix, that is to swap the columns and rows.
            OpenGL developers often use an internal matrix layout called column-major ordering which is the 
            default matrix layout in GLM so there is no need to transpose the matrices; we can keep it at GL_FALSE. 
            The last parameter is the actual matrix data, but GLM stores their matrices not in the exact way that 
            OpenGL likes to receive them so we first transform them with GLM's built-in function value_ptr.
        */
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        object.draw();

        glfwSwapBuffers(window); // will swap the color buffer
        glfwPollEvents(); // function checks if any events are triggered (like keyboard input or mouse movement events) 
    }

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

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

// keyboard
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
