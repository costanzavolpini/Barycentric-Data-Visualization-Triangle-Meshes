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
#include <cstdlib>
#include "glm/ext.hpp" //to test

#define IS_IN_DEBUG false // to show normals

using namespace std;

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int current_width = 800;
int current_height = 600;

// Arcball instance
static Arcball arcball(WIDTH, HEIGHT, 1.5f, true, true);

void error_callback(int error, const char * desc);

// create object
Object object = Object();

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

// ------- TRANSFORMATION -------
static glm::mat4 transform_shader = glm::mat4(1.0f);

// ------- IMGUI -----------
void show_window(bool* p_open, GLFWwindow* window);
bool window_showed = true;
void rotation_settings();
void zoom_settings();

// set-up parameter imgui
static float angle = 180.0f; // angle of rotation - must be the same of transform_shader
static float axis_x = 0.0f; // axis of rotation - must be the same of transform_shader
static float axis_y = 1.0f;  // axis of rotation - must be the same of transform_shader
static float axis_z = 0.0f;  // axis of rotation - must be the same of transform_shader
static bool rotate_animation = false; // animate rotation or not
static bool last_time_was_animated = false; // if was moving and now we have stopped it


// FRAMEBUFFER


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

    // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size:
    int windowWidth = WIDTH;
    int windowHeight = HEIGHT;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

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


    // --------- SET UP ------------

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);

    // Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


    // Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

    // ----------------------

    // --------------- SHADER -------------------------------
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
    object.set_file(name_file); //load mesh
    object.setGaussianCurvature(isGaussianCurvature);
    object.setExtendFlatShading(isExtendFlatShading);
    object.setGouraudFlatShading(isGouraudShading);
    object.setLinearInterpolation(isLinearInterpolation);
    object.init(); // fn to initialize VBO and VAO

    /**
        IMPORTANT FOR TRANSFORMATION:
        Since GLM version 0.9.9, GLM default initializates matrix types to a 0-initalized matrix,
        instead of the identity matrix. From that version it is required to initialize matrix types as: glm::mat4 mat = glm::mat4(1.0f).
    */
    // camera position (eye) - look at origin - head is up
    glm::mat4 view = glm::lookAt(glm::vec3(4.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 model = glm::mat4(1.0f);
    transform_shader = model * glm::rotate(transform_shader, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    ourShader.use(); // glUseProgram


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
    }

    // ---------- END SHADER -----------------


    // ---------------------------------------------
	// Render to Texture - specific code begins here
	// ---------------------------------------------

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint frame_buffer = 0;
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	// The texture we're going to render to
	GLuint rendered_texture;
	glGenTextures(1, &rendered_texture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, rendered_texture);

	// Give an empty image to OpenGL ( the last "0" means "empty" )
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The depth buffer
	GLuint depth_render_buffer;
	glGenRenderbuffers(1, &depth_render_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_render_buffer);

    // Set "rendered_texture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rendered_texture, 0);

    // Set the list of draw buffers.
	GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, draw_buffers); // "1" is the size of draw_buffers

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;


    // The fullscreen quad's FBO
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	// Create and compile our GLSL program from the shaders
    Shader quadShader("bufferTexture.vs", "bufferTexture.fs");
    quadShader.use();
	GLuint texID = glGetUniformLocation(quadShader.shaderProgram, "renderedTexture");
	GLuint timeID = glGetUniformLocation(quadShader.shaderProgram, "time");


    // --------------- IMGUI ---------------------
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // ---------------- END IMGUI ----------------------


    /**
        application to keep drawing images and handling user input until the program has been explicitly told to stop
        render loop
    */
    while(!glfwWindowShouldClose(window)) { // function checks at the start of each loop iteration if GLFW has been instructed to close

        glfwPollEvents();

        // new frame imgui
        ImGui_ImplGlfwGL3_NewFrame();

        // keyboard
        process_input(window);

        glEnable(GL_DEPTH_TEST);

        // Render to our framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glViewport(0, 0, windowWidth, windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right


        // render colours
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the depth buffer before each render iteration (otherwise the depth information of the previous frame stays in the buffer).

        glm::mat4 projection = glm::perspective(glm::radians(Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 10.f);

        ourShader.use();

        // arcball
        glm::mat4 rotated_view = view * arcball.rotation_matrix_view();
        glm::mat4 rotated_model = model * arcball.rotation_matrix_model(rotated_view);

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", rotated_view);
        ourShader.setMat4("model", rotated_model);

        ourShader.setMat4("model", transform_shader);

        object.draw(); // draw
        // object.disable(); // and then disable


        if (IS_IN_DEBUG){
            // then draw model with normal visualizing geometry shader (FOR DEBUG)
            normalShader.use();
            normalShader.setMat4("projection", projection);
            normalShader.setMat4("view", rotated_view);
            normalShader.setMat4("model", rotated_model);

            object.draw(); // draw
            // object.disable();
        }

        // ---------- RENDER AS TEXTURE (FBO) --------------------

        // Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render on the whole framebuffer, complete from the lower left corner to the upper right
		glViewport(0, 0, windowWidth, windowHeight);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
        quadShader.use();

        // Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rendered_texture);

		// // Set our "renderedTexture" sampler to use Texture Unit 0
		// glUniform1i(texID, 0);

		// glUniform1f(timeID, (float)(glfwGetTime() * 10.0f));

        // // 1rst attribute buffer : vertices
		// glEnableVertexAttribArray(0);
		// glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		// glVertexAttribPointer(
		// 	0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		// 	3,                  // size
		// 	GL_FLOAT,           // type
		// 	GL_FALSE,           // normalized?
		// 	0,                  // stride
		// 	(void*)0            // array buffer offset
		// );

		// // Draw the triangles !
		// glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

		// glDisableVertexAttribArray(0);

        // ---------- END RENDER AS TEXTURE (FBO) --------------------

        // ----------- IMGUI -----------------

        // Rendering
        show_window(&window_showed, window);

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        // swap the buffers and check for events
        glfwSwapBuffers(window); // will swap the color buffer
        // glfwPollEvents(); // function checks if any events are triggered (like keyboard input or mouse movement events)
        glDisable(GL_DEPTH_TEST);
    }

    ourShader.deactivate();
    quadShader.deactivate();

    if (IS_IN_DEBUG){
        normalShader.deactivate();
    }

    // delete the shader objects once we've linked them into the program object; we no longer need them anymore
    object.clear();
    glDeleteFramebuffers(1, &frame_buffer);
	glDeleteTextures(1, &rendered_texture);
	glDeleteRenderbuffers(1, &depth_render_buffer);
	glDeleteBuffers(1, &quad_vertexbuffer);

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

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
    current_width = width;
    current_height = height;
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
    cout << "eh" << endl;
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
}

// IMGUI window with specific to set rotation, zoom, examples...
void show_window(bool* p_open, GLFWwindow* window){
        // Window for movement control
            // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
            static bool no_titlebar = true;
            static bool no_scrollbar = false;
            static bool no_menu = true;
            static bool no_move = true;
            static bool no_resize = false;
            static bool no_collapse = false;
            static bool no_close = true;
            static bool no_nav = false;
            static bool no_autoresize = false;


            // Demonstrate the various window flags. Typically you would just use the default.
            ImGuiWindowFlags window_flags = 0;
            if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
            if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
            if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
            if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
            if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
            if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
            if (no_nav)       window_flags |= ImGuiWindowFlags_NoNav;
            if(!no_autoresize) window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
            if (no_close)     p_open = NULL; // Don't pass our bool* to Begin


            ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(WIDTH-10, HEIGHT-10), ImGuiCond_Always);

            ImGui::Begin("Movement control", p_open, window_flags);


            ImGui::Columns(3, "mixed");
            ImGui::Separator();

            ImGui::Text("Settings");
            if (ImGui::CollapsingHeader("Rotation")) {
                rotation_settings();
            }
            if (ImGui::CollapsingHeader("Zoom")) {
                zoom_settings();
            }
            if (ImGui::CollapsingHeader("Models")) {
                ImGui::TextWrapped("This window is being created by the ShowDemoWindow() function. Please refer to the code in imgui_demo.cpp for reference.\n\n");
                ImGui::Text("USER GUIDE:");
                // ImGui::ShowUserGuide();
            }
            if (ImGui::CollapsingHeader("Shading")) {
                ImGui::TextWrapped("This window is being created by the ShowDemoWindow() function. Please refer to the code in imgui_demo.cpp for reference.\n\n");
                ImGui::Text("USER GUIDE:");
                // ImGui::ShowUserGuide();
            }
            ImGui::NextColumn();

            ImGui::Text("ImGui"); // opengl

            //pass the texture of the FBO
            //object.getVAO() is the texture of the FBO
            //the next parameter is the upper left corner for the uvs to be applied at
            //the third parameter is the lower right corner
            //the last two parameters are the UVs
            //they have to be flipped (normally they would be (0,0);(1,1)
            ImGui::GetWindowDrawList()->AddImage((void*)object.getVAO(),
            ImVec2(ImGui::GetCursorScreenPos()),
            ImVec2(ImGui::GetCursorScreenPos().x + WIDTH/2,
            ImGui::GetCursorScreenPos().y + HEIGHT/2), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::NextColumn();

            ImGui::Text("Analyse");
            ImGui::Button("Corniflower");
            static float bar = 1.0f;
            ImGui::InputFloat("blue", &bar, 0.05f, 0, 3);
            ImGui::NextColumn();


            // ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - HEIGHT), 0);
            // ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 0), 0);


            // ImGui::Text("ZOOM = %d", zoom);

            ImGui::End();
}

void rotation_settings(){
            ImGui::Text("Set the angle of rotation:");
            ImGui::SliderFloat("angle", &angle, -360.0f, 360.0f);             // Edit 1 angle from -360 to 360

            ImGui::Text("Set axis of rotation:");

            ImGui::InputFloat("input float", &axis_x, 0.01f, 1.0f);
            ImGui::InputFloat("input float", &axis_y, 0.01f, 1.0f);
            ImGui::InputFloat("input float", &axis_z, 0.01f, 1.0f);

            ImGui::Checkbox("rotate/stop", &rotate_animation);

            if(rotate_animation == true){
                angle = (float)glfwGetTime();
                last_time_was_animated = true;
            }

            if (last_time_was_animated && rotate_animation == false){
                axis_x = 0.0f;
                axis_y = 1.0f;
                axis_z = 0.0f;
                angle = 180;
                last_time_was_animated = false;
            }

            transform_shader = glm::rotate(transform_shader, angle, glm::vec3(axis_x, axis_y, axis_z));
}


void zoom_settings(){
    ImGui::Text("Set how much zoom the object:");
    ImGui::SliderFloat("zoom", &Zoom, 100, 1);             // Zoom
}


