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
void set_shader();
void select_model();

// set-up parameter imgui
static float angle = 180.0f; // angle of rotation - must be the same of transform_shader
static float axis_x = 0.0f; // axis of rotation - must be the same of transform_shader
static float axis_y = 1.0f;  // axis of rotation - must be the same of transform_shader
static float axis_z = 0.0f;  // axis of rotation - must be the same of transform_shader
static bool rotate_animation = false; // animate rotation or not
static bool last_time_was_animated = false; // if was moving and now we have stopped it

static ImVec4 color_imgui = ImColor(0, 0, 0, 255);

int count_angle = 0;
bool decrease_angle = false;

// imgui shaders
static int shader_set = 0;

// imgui listbox models
static int listbox_item_current = 0;
static int listbox_item_prev = 0;

void set_parameters_shader(int selected_shader);

// ------- END IMGUI -------------

// ------ SETTINGS SHADERS ---------------
const char * vertex_shader;
const char * geometry_shader;
const char * fragment_shader;
int imgui_isGaussianCurvature;
int imgui_isLinearInterpolation;
int imgui_isExtendFlatShading;
int imgui_isGouraudShading;
string name_file = "models/armadillo.off"; //default name


// ----------- END SETTINGS SHADERS ----------


int main(int argc, char * argv[]) {
    set_parameters_shader(0);

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
    Shader ourShader = Shader();

    Shader normalShader = Shader();
    normalShader.initialize_shader("normal.vs", "normal.fs", "normal.gs");


    /**
        NB. OpenGL works in 3D space we render a 2D triangle with each vertex having a z coordinate of 0.0.
        This way the depth of the triangle remains the same making it look like it's 2D.

        Send vertex data to vertex shader (load .off file).
     */
    object.set_file(name_file); //load mesh
    object.init(); // fn to initialize VBO and VAO

    /**
        IMPORTANT FOR TRANSFORMATION:
        Since GLM version 0.9.9, GLM default initializates matrix types to a 0-initalized matrix,
        instead of the identity matrix. From that version it is required to initialize matrix types as: glm::mat4 mat = glm::mat4(1.0f).
    */
    // camera position (eye) - look at origin - head is up
    glm::mat4 view = glm::lookAt(glm::vec3(4.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 model = glm::mat4(1.0f);
    transform_shader = glm::rotate(transform_shader, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

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

        ourShader.initialize_shader(vertex_shader, fragment_shader, geometry_shader);
        ourShader.use();

        // --- setting shaders ---
        if(imgui_isExtendFlatShading || imgui_isGouraudShading){
            // get matrix's uniform location and set matrix
            ourShader.setVec3("light.position", 0.5f, 0.5f, 0.5f);
            ourShader.setVec3("viewPos", glm::vec3(0.0f, 0.0f, 3.0f));

            // light properties
            ourShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
            ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
            ourShader.setVec3("light.specular", 0.2f, 0.2f, 0.2f);
            ourShader.setFloat("shininess", 12.0f);

        } else if(imgui_isGaussianCurvature) {

            // gaussian curvature
            ourShader.setFloat("min_gc", object.get_minimum_gaussian_curvature_value());
            ourShader.setFloat("max_gc", object.get_maximum_gaussian_curvature_value());
            ourShader.setFloat("mean_negative_gc", object.get_negative_mean_gaussian_curvature_value());
            ourShader.setFloat("mean_positive_gc", object.get_positive_mean_gaussian_curvature_value());
        }
        // --- end settings shaders ---

        // arcball
        glm::mat4 rotated_view = view * arcball.rotation_matrix_view();
        glm::mat4 rotated_model = model * arcball.rotation_matrix_model(rotated_view);

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", rotated_view);
        // ourShader.setMat4("model", rotated_model);

        // imgui rotation
        if(rotate_animation == true){

            if(!last_time_was_animated)
                count_angle = 0;

            angle = count_angle;

            // if(count_angle > 360){
            //     decrease_angle = true;
            // } else if(count_angle < 0){
            //     decrease_angle = false;
            // }


            // if(decrease_angle)
            //     count_angle--;
            // else
            //     count_angle++;

            if(count_angle > 360)
                count_angle = 0;
            count_angle++;

            transform_shader = glm::rotate(glm::mat4(1.0f), glm::radians((float) angle), glm::vec3(axis_x, axis_y, axis_z));
            last_time_was_animated = true;
        }


        ourShader.setMat4("model", transform_shader);

        object.draw(); // draw

        if (IS_IN_DEBUG){
            // then draw model with normal visualizing geometry shader (FOR DEBUG)
            normalShader.use();
            normalShader.setMat4("projection", projection);
            normalShader.setMat4("view", rotated_view);
            normalShader.setMat4("model", rotated_model);

            object.draw(); // draw
        }

        // ---------- RENDER AS TEXTURE (FBO) --------------------

        // Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

    if (IS_IN_DEBUG){
        normalShader.deactivate();
    }

    // delete the shader objects once we've linked them into the program object; we no longer need them anymore
    object.clear();
    glDeleteFramebuffers(1, &frame_buffer);
	glDeleteTextures(1, &rendered_texture);
	glDeleteRenderbuffers(1, &depth_render_buffer);

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


            // ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - 10.0f, io.DisplaySize.y -10.0f), ImGuiCond_Always);

            float size_x = (float) io.DisplaySize.x/8.0f;
            // ImGui::SetNextWindowBgAlpha(1.0f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, color_imgui);

            ImGui::Begin("Movement control", p_open, window_flags);

            ImGui::Columns(3, "mixed");
            ImGui::Separator();
            ImGui::SetColumnOffset(0, 0);

            // ImGui::PushItemWidth(-1);

            ImGui::Text("Settings");
            if (ImGui::CollapsingHeader("Rotation")) {
                rotation_settings();
            }
            if (ImGui::CollapsingHeader("Zoom")) {
                zoom_settings();
            }
            if (ImGui::CollapsingHeader("Models")) {
                select_model();
            }
            if (ImGui::CollapsingHeader("Shaders")) {
                set_shader();
            }
            ImGui::NextColumn();

            ImGui::Text("ImGui"); // opengl

            ImGui::SetColumnOffset(1, size_x * 2);

            //pass the texture of the FBO
            //object.getVAO() is the texture of the FBO
            //the next parameter is the upper left corner for the uvs to be applied at
            //the third parameter is the lower right corner
            //the last two parameters are the UVs
            //they have to be flipped (normally they would be (0,0);(1,1)
            ImGui::GetWindowDrawList()->AddImage((void*)object.getVAO(),
            ImVec2(ImGui::GetCursorScreenPos()),
            ImVec2(ImGui::GetCursorScreenPos().x + io.DisplaySize.x/2,
            ImGui::GetCursorScreenPos().y + io.DisplaySize.y/2), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::NextColumn();

            ImGui::SetColumnOffset(2, size_x * 6);

            ImGui::Text("Analyse\n\n");
            // ------------------ example analyse ------------------

            static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };

            ImGui::PlotHistogram("", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0,80));


            struct Funcs
            {
                static float Sin(void*, int i) { return sinf(i * 0.1f); }
                static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
            };
            static int func_type = 0, display_count = 70;
            // ImGui::Separator();
            ImGui::PushItemWidth(100); ImGui::Combo("func", &func_type, "Sin\0Saw\0"); ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::SliderInt("Sample count", &display_count, 1, 400);
            float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
            ImGui::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0,80));
            ImGui::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0,80));

            // Animate a simple progress bar
            static float progress = 0.0f, progress_dir = 1.0f;
            progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
            if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
            if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }


            // Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
            ImGui::ProgressBar(progress, ImVec2(0.0f,0.0f));
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("Progress Bar");

            float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
            char buf[32];
            sprintf(buf, "%d/%d", (int)(progress_saturated*1753), 1753);
            ImGui::ProgressBar(progress, ImVec2(0.f,0.f), buf);

            // ------------------ end example analyse ------------------


            ImGui::SetCursorPosY(io.DisplaySize.y-18.0f); // columns end at the end of window
            ImGui::NextColumn();

            ImGui::End();
            ImGui::PopStyleColor();
}

// Function to handle rotation made by GUI
void rotation_settings(){
            float prev_angle = angle;
            float prev_axis_x = axis_x;
            float prev_axis_y = axis_y;
            float prev_axis_z = axis_z;

            ImGui::Text("Set the angle of rotation:");
            ImGui::SliderFloat("angle", &angle, 0.0f, 360.0f);             // Edit 1 angle from 0 to 360

            ImGui::Text("Set axis of rotation:");

            ImGui::InputFloat("x", &axis_x, 0.01f, 1.0f);
            ImGui::InputFloat("y", &axis_y, 0.01f, 1.0f);
            ImGui::InputFloat("z", &axis_z, 0.01f, 1.0f);

            ImGui::Checkbox("rotate/stop", &rotate_animation);

            if (last_time_was_animated && rotate_animation == false){
                axis_x = 0.0f;
                axis_y = 1.0f;
                axis_z = 0.0f;
                angle = 180;
                last_time_was_animated = false;
            }

            if((prev_angle != angle) || (prev_axis_x != axis_x) || (prev_axis_y != axis_y) || (prev_axis_z != axis_z))
                transform_shader = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(axis_x, axis_y, axis_z));
}


// function to set zoom into imgui window
void zoom_settings(){
    ImGui::Text("Zoom-in/out:");
    ImGui::SliderFloat("zoom", &Zoom, 100, 1); // Zoom
}


void set_shader(){
    ImGui::TextWrapped("Set a shader experiment to see how the model looks like:\n\n");
    ImGui::RadioButton("Linear Interpolation", &shader_set, 0);
    ImGui::RadioButton("Extend Flat Shading", &shader_set, 1);
    ImGui::RadioButton("Gouraud Shading", &shader_set, 2);
    ImGui::RadioButton("Gaussian Curvature", &shader_set, 3);
    ImGui::RadioButton("Linear Interpolation GC", &shader_set, 4);

    set_parameters_shader(shader_set);
}


// function to set shader
void set_parameters_shader(int selected_shader){
    switch (selected_shader) {

        case 1: // extend flat shading
        vertex_shader = "vertexShader.vs";
        fragment_shader = "maxDiagramFragmentShader.fs";
        geometry_shader = "geometryShader.gs";

        imgui_isExtendFlatShading = 1;
        imgui_isGaussianCurvature = 0;
        imgui_isGouraudShading = 0;
        imgui_isLinearInterpolation = 0;
        break;

        // ---------

        case 2: // gouraud shading
        vertex_shader = "vertexShader.vs";
        fragment_shader = "fragmentShader.fs";
        geometry_shader = NULL;

        imgui_isGouraudShading = 1;
        imgui_isLinearInterpolation = 0;
        imgui_isExtendFlatShading = 0;
        imgui_isGaussianCurvature = 0;
        break;

        // ---------

        case 3: // gaussian curvature
        vertex_shader = "vertexShaderGC.vs";
        fragment_shader = "maxDiagramFragmentShader.fs";
        geometry_shader = "geometryShader.gs";

        imgui_isGaussianCurvature = 1;
        imgui_isExtendFlatShading = 0;
        imgui_isGouraudShading = 0;
        imgui_isLinearInterpolation = 0;
        break;

        case 4: // linear interpolation Gaussian Curvature
        vertex_shader = "vertexShaderGC.vs";
        fragment_shader = "fragmentShader.fs";
        geometry_shader = NULL;

        imgui_isGaussianCurvature = 1;
        imgui_isGouraudShading = 0;
        imgui_isLinearInterpolation = 0;
        imgui_isExtendFlatShading = 0;
        break;

        default:  // linear interpolation
        vertex_shader = "vertexShaderLI.vs";
        fragment_shader = "fragmentShader.fs";
        geometry_shader = NULL;

        imgui_isLinearInterpolation = 1;
        imgui_isExtendFlatShading = 0;
        imgui_isGouraudShading = 0;
        imgui_isGaussianCurvature = 0;
        break;
    }
}

// function to select a model to render
void select_model(){
    listbox_item_prev = listbox_item_current;
    ImGui::TextWrapped("Select a model to render:\n\n");
    const char* listbox_items[] = { "armadillo", "eight", "genus3", "horse", "icosahedron_0", "icosahedron_1", "icosahedron_2", "icosahedron_3", "icosahedron_4"};
    ImGui::PushItemWidth(-1);
    ImGui::ListBox("", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 10);

    name_file = "models/" + std::string(listbox_items[listbox_item_current]) + ".off"; // generate name file

    // if(listbox_item_current != listbox_item_prev)
        // setup vao and vbo and fbo
}


