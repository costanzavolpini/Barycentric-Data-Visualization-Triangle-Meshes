/**
    ref: https://learnopengl.com
    glfw3: brew install glfw3
    g++ -c main.cpp && gcc -c glad.c && g++ -lglfw glad.o main.o -o main 
    or just use make clean && make && ./main
    Costanza Volpini
*/

#include <iostream>
#include <fstream>
#include "glad.h"
#include "point3.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// List of vertices and triangles
vector<Point3d> v;
struct Triangle { int v[3]; };
vector<Triangle> t;
int num_vertices = 10;
vector<int> v_counter(num_vertices);
vector<Point3d> v_norm;

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// to have the obj in center
float fDistance;
float dNear, dFar;
float r;
float px, py, pz;


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


    /**
        Read .OFF file
    */ 
    ifstream in("models/iCorsi/icosahedron_0.off");
    if (!in) {
        cout<<"\nError reading file."<<endl;
        exit(0);
    }
    string s;
    string offName = "OFF";
    getline(in,s);

    if (s.compare(0,offName.size(),offName)!=0) {
        cout << "This is not a valid OFF file." << endl;
        exit(0);
    }

    int i, num_triangles, dummy;
    in >> num_vertices >> num_triangles >> dummy;

    v.resize(num_vertices);
    for (i = 0; i < num_vertices; i++)
        in >> v[i][0] >> v[i][1] >> v[i][2];

    t.resize(num_triangles);

    for (i = 0; i < num_triangles; i++)
        in >> dummy >> t[i].v[0] >> t[i].v[1] >> t[i].v[2];

    in.close();


    // int v_counter[num_vertices] = {0};
    v_norm.resize(num_vertices);

    // vertices array
    float vertices[num_triangles * 18];

    float minx, miny, minz, maxx, maxy, maxz;

    /**
        code to automatically center an obj:
        1. find max and min for x, y and z
        2. bound the model by a sphere of radius r, centered on a point p
        ref: https://www.opengl.org/discussion_boards/showthread.php/130876-automatically-center-3d-object
    */
    if (num_triangles > 0){
        // initialize values
        minx = v[t[0].v[0]].x();
        miny = v[t[0].v[0]].y();
        minz = v[t[0].v[0]].z();

        maxx = v[t[0].v[0]].x();
        maxy = v[t[0].v[0]].y();
        maxz = v[t[0].v[0]].z();
    }
    
    int index = 0;


    for (int k = 0; k < num_triangles; k++) {    
        Point3d v1 = v[t[k].v[0]];
        Point3d v2 = v[t[k].v[1]];
        Point3d v3 = v[t[k].v[2]];

        // insert values in vertices
        // first vertex
        vertices[index] = v1.x();
        vertices[index + 1] = v1.y();
        vertices[index + 2] = v1.z();

        // find max and min for coord x
        if (v1.x() < minx)
            minx = v1.x();
        
        if (v1.x() > maxx)
            maxx = v1.x();

        // find max and min for coord y
        if (v1.y() < miny)
            miny = v1.y();
        
        if (v1.y() > maxy)
            maxy = v1.y();

        // find max and min for coord z
        if (v1.z() < minz)
            minz = v1.z();
        
        if (v1.z() > maxz)
            maxz = v1.z();

        //color
        vertices[index + 3] = 1.0f; // v1 red
        vertices[index + 4] = 0.0f;
        vertices[index + 5] = 0.0f;

        // second vertex
        vertices[index + 6] = v2.x();
        vertices[index + 7] = v2.y();
        vertices[index + 8] = v2.z();

        // find max and min for coord x
        if (v2.x() < minx)
            minx = v2.x();
        
        if (v2.x() > maxx)
            maxx = v2.x();

        // find max and min for coord y
        if (v2.y() < miny)
            miny = v2.y();
        
        if (v2.y() > maxy)
            maxy = v2.y();

        // find max and min for coord z
        if (v2.z() < minz)
            minz = v2.z();
        
        if (v2.z() > maxz)
            maxz = v2.z();

        //color
        vertices[index + 9] = 0.0f;
        vertices[index + 10] = 1.0f; // v2 green
        vertices[index + 11] = 0.0f;

        // third vertex
        vertices[index + 12] = v3.x();
        vertices[index + 13] = v3.y();
        vertices[index + 14] = v3.z();
        
        // find max and min for coord x
        if (v3.x() < minx)
            minx = v3.x();
        
        if (v3.x() > maxx)
            maxx = v3.x();

        // find max and min for coord y
        if (v3.y() < miny)
            miny = v3.y();
        
        if (v3.y() > maxy)
            maxy = v3.y();

        // find max and min for coord z
        if (v3.z() < minz)
            minz = v3.z();
        
        if (v3.z() > maxz)
            maxz = v3.z();

        //color
        vertices[index + 15] = 0.0f;
        vertices[index + 16] = 0.0f;
        vertices[index + 17] = 1.0f; // v3 blue    

        index += 18;

        // normal of a triangle
        Point3d n = (v2-v1)^(v3-v1);
        n.normalize();
    }

    index -= 1; // since we added before 18 but we have used only 17 elements

    // bound the model by a sphere of radius r, centered on a point p
    px = (minx + maxx) / 2.0f;
    py = (miny + maxy) / 2.0f;
    pz = (minz + maxz) / 2.0f;
    r = sqrt((maxx - px) * (maxx - px) + (maxy - py) * (maxy - py) + (maxz - pz) * (maxz - pz));

    fDistance = r / 0.57735f; // where 0.57735f is tan(30 degrees);

    // The near and far clipping planes then lay either side of this point, allowing for the radius of the sphere 
    dNear = fDistance - r;
    dFar = fDistance + r;
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float))); // 72-bit floating point values, each position is composed of 6 of those values (3 points + 3 colours (one for each vertex))
    glEnableVertexAttribArray(0); //this 0 is referred to the layout on shader

    // color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); //this 2 is referred to the layout on shader



    /**
        You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    */
    glBindVertexArray(0); 

    cout<<"Scene initialized..."<<endl;

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

        // get matrix's uniform location and set matrix
        ourShader.use(); //draw

        /**
            IMPORTANT FOR TRANSFORMATION:
            Since GLM version 0.9.9, GLM default initializates matrix types to a 0-initalized matrix, 
            instead of the identity matrix. From that version it is required to initialize matrix types as: glm::mat4 mat = glm::mat4(1.0f). 
        */ 
        //frustum
        //glm::perspective = field of view (zoom), aspect (height of frustum), near plane, far plane
        float zoom = 5.0f;
        glm::mat4 frustum = glm::perspective(fDistance * zoom, 2.0f * r, dNear, dFar);
        ourShader.setMat4("projection", frustum);

        //view
        glm::mat4 view = glm::mat4(1.0f);
        // The glm::LookAt function requires a camera position, target/position the camera should look at and up vector that represents the up vector in world space.
        // (eyeX, eyeY, eyeZ) (centerX, centerY, centerZ) (upX, upY, upZ)
        view = glm::lookAt(glm::vec3(0.0, 0.0, fDistance), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("view", view);

        // create transformations
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(-px, -py, -pz));
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

        /**
            Every shader and rendering call after glUseProgram will now use this program object (and thus the shaders).
            The output of the geometry shader is then passed on to the rasterization stage where it maps the resulting primitive(s) 
            to the corresponding pixels on the final screen, resulting in fragments for the fragment shader to use.  
            + Clipping (discards all fragments that are outside your view, increasing performance).
        */
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, num_triangles * 3);


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
