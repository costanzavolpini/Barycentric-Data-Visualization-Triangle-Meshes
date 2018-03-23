#ifndef OBJMODEL_H
#define OBJMODEL_H

#include "Point3.h"
#include "Base.h"
#include <math.h>
#include "LoaderObject.h"

using namespace std;

/***************************************************************************
Object.h
Comment:  This file contains all Object definitions to construct and draw an object.
***************************************************************************/

class Object {
  public:
    vector<float> vertices; // vector containing all vertices and colors
    vector<Point3d> normals;
    vector<float> fnormals;
    
    /**
        Memory on the GPU where we store the vertex data
        VBO: manage this memory via so called vertex buffer objects (VBO) that can store a large number of vertices in the GPU's memory
    */
    unsigned int VBO, VAO, VBO_NORMAL, VBO_LAMP, VAO_LAMP;

    // Constructor
      Object(const std::string &_path) {
        if(!load(_path.c_str(), vertices, normals)){
            cout << "error loading file" << endl;
            return;
        }
        vecPoint3dToFloat(normals, fnormals);
      }

     // Function to initialize VBO and VAO
      void init() {

          // ------------- VBO -------------
          // Use VBO to avoid to send data vertex at a time (we send everything together)
          glGenBuffers(1, &VBO); //generate buffer, bufferID = 1

          glBindBuffer(GL_ARRAY_BUFFER, VBO); 

          /**
              GL_STATIC_DRAW: the data will most likely not change at all or very rarely.
              GL_DYNAMIC_DRAW: the data is likely to change a lot.
              GL_STREAM_DRAW: the data will change every time it is drawn.
          */
          glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW); // copies the previously defined vertex data into the buffer's memor

          // VBO NORMALS
          glGenBuffers(1, &VBO_NORMAL); //generate buffer, bufferID = 1

          glBindBuffer(GL_ARRAY_BUFFER, VBO_NORMAL); 

          glBufferData(GL_ARRAY_BUFFER, sizeof(float) * fnormals.size(), &fnormals[0], GL_STATIC_DRAW);

          // ------------- VAO -------------
          glGenVertexArrays(1, &VAO);

          /**
              VAO: when configuring vertex attribute pointers you only have to make those calls once and 
              whenever we want to draw the object, we can just bind the corresponding VAO
          */
          glBindVertexArray(VAO); // If we fail to bind a VAO, OpenGL will most likely refuse to draw anything.

          /**
              void glVertexAttribPointer(GLuint index​, GLint size​, GLenum type​, GLboolean normalized​, GLsizei stride​, const GLvoid * pointer​);
              size = 6 since we want to pass 3 values (it is a vec3) and a colour for each vertex.
              stride tells us the space between consecutive vertex attribute sets
              offset of where the position data begins in the buffer. Since the position data is at the start of the data array this value is just 0. 
          */

          glBindBuffer(GL_ARRAY_BUFFER, VBO);
          //position attribute
          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float))); // 72-bit floating point values, each position is composed of 6 of those values (3 points + 3 colours (one for each vertex))
          glEnableVertexAttribArray(0); //this 0 is referred to the layout on shader

          glBindBuffer(GL_ARRAY_BUFFER, VBO_NORMAL);
          //normal attribute
          glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float))); 
          glEnableVertexAttribArray(1); //this 1 is referred to the layout on shader

          /**
            Unbind the VAO so other VAO calls won't accidentally modify this VAO, but this rarely happens. 
            Modifying other VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
          */
          glBindVertexArray(0); 

          cout << "Scene initialized..." << endl;
      }

      // function to draw the triangles of the mesh
      // it must be called after that we have called glUseProgram on shader.
      void draw(){
          /**
            The output of the geometry shader is then passed on to the rasterization stage where it maps the resulting primitive(s) 
            to the corresponding pixels on the final screen, resulting in fragments for the fragment shader to use.  
            + Clipping (discards all fragments that are outside your view, increasing performance).
        */
        int num_triangles = vertices.size() / 18;

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, num_triangles * 3);
      }


     // delete the shader objects once we've linked them into the program object; we no longer need them anymore
      void clear(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);

      }
};

#endif
