/**
    ref: https://learnopengl.com
    glfw3: brew install glfw3
    g++ -c main.cpp && gcc -c glad.c && g++ -lglfw glad.o main.o -o main 
    or just use make clean && make && ./main
    Costanza Volpini
*/

#include "Base.h"
#include "Shader.h"
#include "Camera.h"
#include "Object.h"




using namespace std;

    // settings
    const unsigned int WIDTH = 800;
    const unsigned int HEIGHT = 600;

    // Camera settings
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    float lastX = WIDTH / 2.0f;
    float lastY = HEIGHT / 2.0f;
    bool firstMouse = true;

    // Timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    // resize window
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    // keyboard
    void processInput(GLFWwindow *window);

    double last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;
    int arcball_on = false;

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    glm::vec3 get_arcball_vector(double x, double y);
    

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
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
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
    Shader ourShader("vertexShader.vs", "maxDiagramFragmentShader.fs");
    Shader lampShader("lampVertexShader.vs", "lampFragmentShader.fs");

    /**
        NB. OpenGL works in 3D space we render a 2D triangle with each vertex having a z coordinate of 0.0.
        This way the depth of the triangle remains the same making it look like it's 2D.
        
        Send vertex data to vertex shader (load .off file). 
     */ 
    
    Object object = Object("models/iCorsi/icosahedron_0.off");
    object.init();

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the depth buffer before each render iteration (otherwise the depth information of the previous frame stays in the buffer).


        // get matrix's uniform location and set matrix
        ourShader.use(); //draw
        ourShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f); 
        ourShader.setVec3("viewPos", camera.Position);

        // light properties
        ourShader.setVec3("light.ambient", 1.2f, 1.2f, 1.2f);
        ourShader.setVec3("light.diffuse", 1.5f, 1.5f, 1.5f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("shininess", 32.0f);

        /**
            IMPORTANT FOR TRANSFORMATION:
            Since GLM version 0.9.9, GLM default initializates matrix types to a 0-initalized matrix, 
            instead of the identity matrix. From that version it is required to initialize matrix types as: glm::mat4 mat = glm::mat4(1.0f). 
        */ 
        //frustum
        //glm::perspective = field of view (zoom), aspect (height of frustum), near plane, far plane
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        // The glm::LookAt function requires a camera position, target/position the camera should look at and up vector that represents the up vector in world space.
        // camera.GetViewMatrix call a LookAt with: (eyeX, eyeY, eyeZ) (centerX, centerY, centerZ) (upX, upY, upZ)
        view = camera.GetViewMatrix();

        ourShader.setMat4("projection", projection);

        // create transformations
        glm::mat4 model = glm::mat4(1.0f);
        
        // arcball move
        if (cur_mx != last_mx || cur_my != last_my) {
            glm::vec3 va = get_arcball_vector(last_mx, last_my); //OP1
            glm::vec3 vb = get_arcball_vector(cur_mx,  cur_my); //OP2
            float angle = acos(min(1.0f, glm::dot(va, vb)));
            glm::vec3 axis_in_camera_coord = glm::cross(va, vb);

            // converting the rotation axis from camera coordinates to object coordinates. 
            model = glm::translate(model,  glm::vec3( 0.0f,  0.0f,  0.0f));
            glm::mat3 camera2object = glm::inverse(glm::mat3(camera.GetViewMatrix()) * glm::mat3(model)); //from camera to object coord
            glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;

            // rotation axis from object coordinates to world coordinates
            model = glm::rotate(model, glm::degrees(angle), axis_in_object_coord);
            last_mx = cur_mx;
            last_my = cur_my;
        }

        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        object.draw();

        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", glm::mat4(1.0f));
        lampShader.setMat4("model", glm::mat4(1.0f));

        object.drawLight();

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

// Function that activate arcball when button left is pressed.
// mouse button, button action and modifier bits.
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        arcball_on = true;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        last_mx = cur_mx = xpos;
        last_my = cur_my = ypos;
        cout << "left " << cur_mx << endl;
        cout << "left " << cur_my << endl;
    } else
        arcball_on = false;
}

// Function that take position when arcball is activate (left button is pressed)
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
  if (arcball_on) {  // if left button is pressed
    cur_mx = xpos;
    cur_my = ypos;
    cout << cur_mx << endl;
    cout << cur_my << endl;
  }
}

// void onMouse(int button, int state, int x, int y) {
//   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
//     arcball_on = true;
//     last_mx = cur_mx = x;
//     last_my = cur_my = y;
//   } else {
//     arcball_on = false;
//   }
// }

// void onMotion(int x, int y) {
//   if (arcball_on) {  // if left button is pressed
//     cur_mx = x;
//     cur_my = y;
//   }
// }

// keyboard
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);



    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    //     camera.ProcessKeyboard(UP, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    //     camera.ProcessKeyboard(BOTTOM, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    //     camera.ProcessKeyboard(LEFT, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    //     camera.ProcessKeyboard(RIGHT, deltaTime);


}


/**
    Get a normalized vector from the center of the virtual ball O to a
    point P on the virtual ball surface, such that P is aligned on
    screen's (X,Y) coordinates.  If (X,Y) is too far away from the
    sphere, return the nearest point on the virtual ball surface.
    https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
 */
glm::vec3 get_arcball_vector(double x, double y) {
// convert the x,y screen coordinates to [-1,1] coordinates
  glm::vec3 P = glm::vec3(1.0 * (x/WIDTH) * 2 - 1.0,
			  1.0 * (y/HEIGHT) * 2 - 1.0,
			  0);
  P.y = -P.y;
  float OP_squared = P.x * P.x + P.y * P.y;

  // use the Pythagorean theorem to check the length of the OP vector and compute the z coordinate
  if (OP_squared <= 1 * 1)
    P.z = sqrt(1 * 1 - OP_squared);  // Pythagore
  else
    P = glm::normalize(P);  // nearest point
  return P;
}

