#ifndef OBJMODEL_H
#define OBJMODEL_H

#include "Point3.h"
#include "Base.h"
#include <math.h>
#include "LoaderObject.h"
#include "kPercentileHelper.h"

#include <numeric> // TODO: remove after having debugged the mean curvature

using namespace std;

/***************************************************************************
Object.h
Comment:  This file contains all Object definitions to construct and draw an object.
***************************************************************************/

//TODO: fix mean curvature when change model -> not work (maybe)
class Object
{
  public:
    vector<float> triangle_vertices;
    vector<float> triangle_normals;
    vector<float> triangle_gc; //untouched gc
    vector<float> triangle_mc; //mean curvature per edges
    vector<float> triangle_mc_vertex; // mean curvature per vertex

    vector<float> triangle_gc_modified_auto; //outliers gc
    vector<float> triangle_gc_vertex; // vector of gaussian curvature of length vertex

    vector<float> triangle_mc_modified_auto; //outliers mc
    vector<float> triangle_mc_edge; // vector of gaussian curvature of length edge

    double best_min_gc;
    double best_max_gc;

    double best_min_mc;
    double best_max_mc;

    int type_gc = 2;

    KPercentile k_percentile_gc = KPercentile();
    KPercentile k_percentile_mc = KPercentile();

    /**
        Memory on the GPU where we store the vertex data
        VBO: manage this memory via so called vertex buffer objects (VBO) that can store a large number of vertices in the GPU's memory
    */
    unsigned int VBO, VAO, VBO_NORMAL, VBO_GAUSSIANCURVATURE, VBO_MEANCURVATURE;

    // Constructor
    void set_file(const std::string &_path)
    {
        triangle_vertices.clear();
        triangle_normals.clear();
        triangle_gc.clear();
        triangle_mc.clear();
        triangle_mc_vertex.clear();
        triangle_gc_vertex.clear();
        triangle_gc_modified_auto.clear();

        triangle_vertices.shrink_to_fit();
        triangle_normals.shrink_to_fit();
        triangle_gc.shrink_to_fit();
        triangle_mc.shrink_to_fit();
        triangle_mc_vertex.shrink_to_fit();
        triangle_gc_vertex.shrink_to_fit();
        triangle_gc_modified_auto.shrink_to_fit();


        if (!load(_path.c_str(), triangle_vertices, triangle_normals, triangle_gc, triangle_mc, triangle_mc_vertex, triangle_gc_vertex, triangle_mc_edge))
        {
            cout << "error loading file" << endl;
            return;
        }
    }


    // Function to initialize VBO and VAO
    // name file and the second it is the method gc
    void init()
    {
        cout << "MAX " << *max_element(triangle_mc_vertex.begin(), triangle_mc_vertex.end()) << endl;
        cout << "MIN " << *min_element(triangle_mc_vertex.begin(), triangle_mc_vertex.end()) << endl;

        vector<double> percentiles_gc = k_percentile_gc.init(triangle_gc_vertex);
        best_min_gc = percentiles_gc[0];
        best_max_gc = percentiles_gc[1];

        vector<double> percentiles_mc = k_percentile_mc.init(triangle_mc_edge);
        best_min_mc = percentiles_mc[0];
        best_max_mc = percentiles_mc[1];
        // modify_mc_temp();
        // triangle_mc = triangle_mc_modified_auto; //FIXME: if this is removed then use: vec4(hsv2rgb(interpolation(green, blue, min(val/max_mc, 1.0))), 1.0); into the vertexShaderMC

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

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * triangle_gc.size(), &triangle_gc[0], GL_STATIC_DRAW);

        // VBO_MEANCURVATURE
        glGenBuffers(1, &VBO_MEANCURVATURE); //generate buffer, bufferID = 1

        glBindBuffer(GL_ARRAY_BUFFER, VBO_MEANCURVATURE);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * triangle_mc.size(), &triangle_mc[0], GL_STATIC_DRAW);

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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0 * sizeof(float))); // 72-bit floating point values, each position is composed of 3 of those values (3 points (one for each vertex))
        glEnableVertexAttribArray(0);                                                                    //this 0 is referred to the layout on shader

        // normals
        glBindBuffer(GL_ARRAY_BUFFER, VBO_NORMAL);
        //normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0 * sizeof(float)));
        glEnableVertexAttribArray(1); //this 1 is referred to the layout on shader

        // gaussian curvature
        glBindBuffer(GL_ARRAY_BUFFER, VBO_GAUSSIANCURVATURE);
        //normal attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0 * sizeof(float)));
        glEnableVertexAttribArray(2); //this 2 is referred to the layout on shader

        // mean curvature
        glBindBuffer(GL_ARRAY_BUFFER, VBO_MEANCURVATURE);
        //normal attribute
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0 * sizeof(float)));
        glEnableVertexAttribArray(3); //this 2 is referred to the layout on shader

        /**
            Unbind the VAO so other VAO calls won't accidentally modify this VAO, but this rarely happens.
            Modifying other VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
          */
        glBindVertexArray(0);

        cout << "Scene initialized..." << endl;
    }

    // function to draw the triangles of the mesh
    // it must be called after that we have called glUseProgram on shader.
    void draw()
    {
        /**
            The output of the geometry shader is then passed on to the rasterization stage where it maps the resulting primitive(s)
            to the corresponding pixels on the final screen, resulting in fragments for the fragment shader to use.
            + Clipping (discards all fragments that are outside your view, increasing performance).
        */
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, triangle_vertices.size());
        glBindVertexArray(0);
    }

    void disable()
    {
        glDisableVertexAttribArray(1);
    }

    // delete the shader objects once we've linked them into the program object; we no longer need them anymore
    void clear()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &VBO_NORMAL);
        glDeleteBuffers(1, &VBO_GAUSSIANCURVATURE);
        glDeleteBuffers(1, &VBO_MEANCURVATURE);
    }

    // Point3d interpolation(Point3d v0, Point3d v1, float t) {
    //     return (1 - t) * v0 + t * v1;
    // }

    /**
     * Get the minimum value of gaussian curvature
    */
    float get_minimum_gaussian_curvature_value()
    {
        return *min_element(triangle_gc.begin(), triangle_gc.end());
    }

    /**
     * Get the maximum value of gaussian curvature
    */
    float get_maximum_gaussian_curvature_value()
    {
        return *max_element(triangle_gc.begin(), triangle_gc.end());
    }

    /**
     * Get the minimum value of mean curvature
    */
    float get_minimum_mean_curvature_value()
    {
        return *min_element(triangle_mc.begin(), triangle_mc.end());
    }

    /**
     * Get the maximum value of mean curvature
    */
    float get_maximum_mean_curvature_value()
    {
        return *max_element(triangle_mc.begin(), triangle_mc.end());
    }


    unsigned int getVAO()
    {
        return VAO;
    }

    vector<double> get_best_values_gc(){
        return vector<double>{best_min_gc, best_max_gc};
    }

    vector<double> get_best_values_mc(){
        return vector<double>{best_min_mc, best_max_mc};
    }

    // TODO: check current mean curvature. This is used to sed the mean at the real mean instead of zero in order to debug and find the problem.
    void modify_mc_temp(){
        double sum = std::accumulate(triangle_mc.begin(), triangle_mc.end(), 0.0);
        double mean = sum / triangle_mc.size();

        double sq_sum = std::inner_product(triangle_mc.begin(), triangle_mc.end(), triangle_mc.begin(), 0.0);
        double stdev = std::sqrt(sq_sum / triangle_mc.size() - mean * mean);

        // remap value triangle_mc
        for(int i = 0; i < triangle_mc.size(); i++){
            if(triangle_mc[i] > mean){
                triangle_mc_modified_auto.push_back(triangle_mc[i]/best_max_mc);
            }else{
                triangle_mc_modified_auto.push_back(triangle_mc[i]/best_min_mc);
            }
        }
    }

    double get_min_mean_vertex(){
        return *min_element(triangle_mc_vertex.begin(), triangle_mc_vertex.end());
    }

    double get_max_mean_vertex(){
        return *max_element(triangle_mc_vertex.begin(), triangle_mc_vertex.end());
    }
};

#endif
