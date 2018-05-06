#ifndef OBJMODEL_H
#define OBJMODEL_H

#include "Point3.h"
#include "Base.h"
#include <math.h>
#include "LoaderObject.h"
#include "GaussianCurvatureHelper.h"

using namespace std;

/***************************************************************************
Object.h
Comment:  This file contains all Object definitions to construct and draw an object.
***************************************************************************/

class Object {
  public:
    vector<float> triangle_vertices;
    vector<float> triangle_normals;
    vector<float> triangle_gc; //untouched gc
    vector<float> triangle_color;

    vector<float> triangle_gc_modified_auto; //outliers gc
    vector<float> triangle_gc_modified; //user modified gc
    vector<float> triangle_gc_selected;
    int type_gc = 2;
    GCHelper gc_helper = GCHelper();

    /**
        Memory on the GPU where we store the vertex data
        VBO: manage this memory via so called vertex buffer objects (VBO) that can store a large number of vertices in the GPU's memory
    */
    unsigned int VBO, VAO, VBO_NORMAL, VBO_GAUSSIANCURVATURE, VBO_LINEARINTERPOLATION;

    // Constructor
      void set_file(const std::string &_path) {
        triangle_vertices.clear();
        triangle_normals.clear();
        triangle_gc.clear();
        triangle_color.clear();

        triangle_vertices.shrink_to_fit();
        triangle_normals.shrink_to_fit();
        triangle_gc.shrink_to_fit();
        triangle_color.shrink_to_fit();

        if(!load(_path.c_str(), triangle_vertices, triangle_normals, triangle_gc, triangle_color)){
            cout << "error loading file" << endl;
            return;
        }

        triangle_gc_modified_auto.clear();
        triangle_gc_modified.clear();
        triangle_gc_selected.clear();

        triangle_gc_modified_auto.shrink_to_fit();
        triangle_gc_modified.shrink_to_fit();
        triangle_gc_selected.shrink_to_fit();

        auto_detect_outliers_gc(); // auto_detect_outliers_gc
        set_selected_gc(); // set_selected_gc
        init(); //init
      }

      void auto_detect_outliers_gc(){
        gc_helper.clear_datas();
        // autodetect gaussian curvature outliers, variance...etc.
        int number_triangles = triangle_gc.size()/9;

        // add everything to triangle gaussian curvature
        for(int k = 0; k < number_triangles; k++){
                gc_helper.update_statistics_data(triangle_gc[9*k]);
                gc_helper.update_statistics_data(triangle_gc[9*k + 3]);
                gc_helper.update_statistics_data(triangle_gc[9*k + 6]);
        }

        gc_helper.finalize_statistics_data();

        triangle_gc_modified_auto.resize(triangle_gc.size());

        for(int k = 0; k < number_triangles; k++){ // update to remove noisy (smoothing)
                triangle_gc_modified_auto[9*k] = triangle_gc_modified_auto[9*k + 1] = triangle_gc_modified_auto[9*k + 2] = gc_helper.cut_data_gaussian_curvature(triangle_gc[9*k]);
                triangle_gc_modified_auto[9*k + 3] = triangle_gc_modified_auto[9*k + 4] = triangle_gc_modified_auto[9*k + 5] = gc_helper.cut_data_gaussian_curvature(triangle_gc[9*k + 3]);
                triangle_gc_modified_auto[9*k + 6] = triangle_gc_modified_auto[9*k + 7] = triangle_gc_modified_auto[9*k + 8] = gc_helper.cut_data_gaussian_curvature(triangle_gc[9*k + 6]);
        }
        triangle_gc_selected = triangle_gc_modified_auto;
      }

     // Function to initialize VBO and VAO
     // name file and the second it is the method gc
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
          glBufferData(GL_ARRAY_BUFFER, sizeof(float) * triangle_vertices.size(), &triangle_vertices[0], GL_STATIC_DRAW); // copies the previously defined vertex data into the buffer's memor

          // VBO NORMALS
          glGenBuffers(1, &VBO_NORMAL); //generate buffer, bufferID = 1

          glBindBuffer(GL_ARRAY_BUFFER, VBO_NORMAL);

          glBufferData(GL_ARRAY_BUFFER, sizeof(float) * triangle_normals.size(), &triangle_normals[0], GL_STATIC_DRAW);


           // VBO_GAUSSIANCURVATURE
           glGenBuffers(1, &VBO_GAUSSIANCURVATURE); //generate buffer, bufferID = 1

           glBindBuffer(GL_ARRAY_BUFFER, VBO_GAUSSIANCURVATURE);

           glBufferData(GL_ARRAY_BUFFER, sizeof(float) * triangle_gc_selected.size(), &triangle_gc_selected[0], GL_STATIC_DRAW);

            // VBO_LINEARINTERPOLATION
            glGenBuffers(1, &VBO_LINEARINTERPOLATION); //generate buffer, bufferID = 1

            glBindBuffer(GL_ARRAY_BUFFER, VBO_LINEARINTERPOLATION);

            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * triangle_color.size(), &triangle_color[0], GL_STATIC_DRAW);


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
          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float))); // 72-bit floating point values, each position is composed of 3 of those values (3 points (one for each vertex))
          glEnableVertexAttribArray(0); //this 0 is referred to the layout on shader


          // normals
          glBindBuffer(GL_ARRAY_BUFFER, VBO_NORMAL);
          //normal attribute
          glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
          glEnableVertexAttribArray(1); //this 1 is referred to the layout on shader


          // gaussian curvature
          glBindBuffer(GL_ARRAY_BUFFER, VBO_GAUSSIANCURVATURE);
          //normal attribute
          glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
          glEnableVertexAttribArray(2); //this 2 is referred to the layout on shader


          // linear interpolation
          glBindBuffer(GL_ARRAY_BUFFER, VBO_LINEARINTERPOLATION);
          // color
          glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
          glEnableVertexAttribArray(3); //this 3 is referred to the layout on shader

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
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, triangle_vertices.size());
        glBindVertexArray(0);
      }

      void disable(){
        glDisableVertexAttribArray(1);
      }


     // delete the shader objects once we've linked them into the program object; we no longer need them anymore
      void clear(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &VBO_NORMAL);
        glDeleteBuffers(1, &VBO_GAUSSIANCURVATURE);
        glDeleteBuffers(1, &VBO_LINEARINTERPOLATION);
      }

    // Point3d interpolation(Point3d v0, Point3d v1, float t) {
    //     return (1 - t) * v0 + t * v1;
    // }

    /**
     * Get the minimum value of gaussian curvature
    */
    float get_minimum_gaussian_curvature_value(){
        return *min_element(triangle_gc.begin(), triangle_gc.end());
    }

    /**
     * Get the maximum value of gaussian curvature
    */
    float get_maximum_gaussian_curvature_value(){
        return *max_element(triangle_gc.begin(), triangle_gc.end());
    }

    /**
     * Get the mean of positive values of gaussian curvature
    */
    float get_positive_mean_gaussian_curvature_value(){
        float sum = 0.0;
        int count = 0;
        for(int k = 0; k < triangle_gc.size(); k++){

        float val = triangle_gc[k]; // gaussian_curvature is a vec3 composed by same value
        if(val > 0){
            sum += abs(val);
            count++;
        }

        //    Point3d red = Point3d(1.0, 0.0, 0.0);
        //    Point3d green = Point3d(0.0, 1.0, 0.0);
        //    Point3d blue = Point3d(0.0, 0.0, 1.0);

        //    if (val < 0) { //negative numbers until 0 -> map from red to green
        //       //  std::cout << interpolation(red, green, val/(*min_element(triangle_gc.begin(), triangle_gc.end()))) << std::endl;
        //     } else { //map from green to blue, from 0 to positive
        //       //  std::cout << interpolation(green, blue, val/(*max_element(triangle_gc.begin(), triangle_gc.end()))) << std::endl;
        //     }
        }
        return sum / count;
    }

    /**
     * Get the mean of negative values of gaussian curvature
    */
    float get_negative_mean_gaussian_curvature_value(){
        float sum = 0.0;
        int count = 0;
        for(int k = 0; k < triangle_gc.size(); k++){

        float val = triangle_gc[k]; // gaussian_curvature is a vec3 composed by same value
        if(val < 0){
            sum += abs(val);
            count++;
        }

        }
        return sum / count;
    }

    unsigned int getVAO(){
        return VAO;
    }


    vector<float> change_values_gaussian_curvature(float max, float min){
            return triangle_gc_modified;
    }

    // function to select the current gc
    void set_selected_gc(){
        switch (type_gc) {
            case 1: // untouched gc
                triangle_gc_selected = triangle_gc;
                break;

            case 3: //modified by user
                triangle_gc_selected = triangle_gc_modified;
                break;


            default: //automatic
                triangle_gc_selected = triangle_gc_modified_auto;
                break;
        }
    }

    void set_value_gc(int val){
        type_gc = val;
    }
};

#endif
