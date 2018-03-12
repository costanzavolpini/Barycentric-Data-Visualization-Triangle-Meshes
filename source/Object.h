#ifndef OBJMODEL_H
#define OBJMODEL_H

#include "Point3.h"
#include "Base.h"
#include <math.h>
#include "LoaderObject.h"

using namespace std;


class Object {
  public:
    vector<float> vertices;
    vector<float> normals;
    
    /**
        Create memory on the GPU where we store the vertex data, configure how OpenGL should interpret the memory and 
        specify how to send the data to the graphics card.
        VBO: manage this memory via so called vertex buffer objects (VBO) that can store a large number of vertices in the GPU's memory
    */
    // GLuint VBO;
    // GLuint VAO;
    unsigned int VBO, VAO;

      Object(const std::string &_path) {
        if(!load(_path.c_str(), vertices, normals)){
            cout << "error loading file" << endl;
            return;
        }
      }

      void init() {
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
          glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW); // copies the previously defined vertex data into the buffer's memor
          
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
      }

      void draw(){
          /**
            Every shader and rendering call after glUseProgram will now use this program object (and thus the shaders).
            The output of the geometry shader is then passed on to the rasterization stage where it maps the resulting primitive(s) 
            to the corresponding pixels on the final screen, resulting in fragments for the fragment shader to use.  
            + Clipping (discards all fragments that are outside your view, increasing performance).
        */
        int num_triangles = vertices.size() / 18;

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, num_triangles * 3);
      }
};

#endif
