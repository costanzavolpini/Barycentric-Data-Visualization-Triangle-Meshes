#ifndef LOADER_H
#define LOADER_H

#include "Point3.h"
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <iterator>
#include <math.h>       /* fmin, sqrt */
#include "glm/ext.hpp"

using namespace std;


/***************************************************************************
LoaderObject.h
Comment:  This file load the mesh using an .OFF file.
***************************************************************************/

// List of vertices and triangles
vector<Point3d> v;
struct Triangle { int v[3]; };
vector<Triangle> t;
double PI = atan(1)*4;
int num_triangles;

double min_coord;
double max_coord;

int interval = 2; // max - min = 1 - (-1)

        /**
         * Function to update the minimum value found in coords of an object.
        */
        void set_min(Point3d current){ // only negative values
            if(!min_coord){
                min_coord = fmin(fmin(current.x(), current.y()), current.z());
                return;
            }
            min_coord = fmin(fmin(current.x(), current.y()), fmin(current.z(), min_coord));
        }

        /**
         * Function to update the maximum value found in coords of an object.
        */
        void set_max(Point3d current){
            if(!max_coord){
                max_coord = fmax(fmax(current.x(), current.y()), current.z());
                return;
            }
            max_coord = fmax(fmax(current.x(), current.y()), fmax(current.z(), max_coord));
        }

        /**
         * Function to get the maximum value found in coords of an object.
        */
        double get_max_coord(){
            return max_coord;
        }

        /**
         * Function to get the minimum value found in coords of an object.
        */
        double get_min_coord(){
            return min_coord;
        }

        /**
         * Function to rescale a coord such that the coords is in a range between -1 and 1.
        */
        Point3d get_rescaled_value(Point3d value){
            return interval/(max_coord - min_coord) * (value - max_coord) + 1; //1 is the max of interval
        }


        int count_data = 0.0f;
        double mean = 0.0f;
        double M2 = 0.0f;
        double variance;
        double standard_deviation;
        double multiplier = 1.5f; // 3 to get extreme case (usually is 1.5)
        double lower_outlier;
        double upper_outlier;

        /**
         * Function for update a new_value, compute the count, new mean, the new M2.
         * mean accumulates the mean of the entire dataset
         * M2 aggregates the squared distance from the mean
         * count aggregates the number of samples seen so far
        */
       void update_statistics_data(double new_value){
           count_data++;
           double delta = new_value - mean;
           mean = mean + (double) delta/count_data;
           double delta2 = new_value - mean;
           M2 = M2 + delta * delta2;
       }


        /**
         * Function for calculate the final mean and variance from a dataset.
        */
       void finalize_statistics_data(){
           if(count_data < 2){ // error case, that means that we have just one data
               mean = 0.0f;
               M2 = 0.0f;
               return;
           }

           variance = (double) M2/(count_data - 1);
           M2 = variance;
           standard_deviation = sqrt(variance);
           lower_outlier = mean - (standard_deviation * multiplier);
           upper_outlier = mean + (standard_deviation * multiplier);
       }


        /**
         * Function for cut data of gaussian curvature respect upper and lower outlier.
        */
       double cut_data_gaussian_curvature(double val){
           if(val > upper_outlier)
                val = upper_outlier;
           else if(val < lower_outlier)
                val = lower_outlier;

            // interval from 0 to 1 then it is 1 because (0+1)
            // I have mapped all values from 0 to 1
            return 1/(upper_outlier - lower_outlier) * (val - upper_outlier) + 1; //1 is the max of interval
       }


        bool load (const char * path, vector<float> &out_vertices, vector<float> &out_normals, vector<float> &gc, vector<float> &color_li) {
            // --------------------- COMPUTATIONS -----------------------------
            /**
                Read .OFF file
            */
            ifstream in(path); // substitute with path
            if (!in) {
                cout<<"\nError reading file."<<endl;
                return false;
            }
            string s;
            string offName = "OFF";
            getline(in,s);

            if (s.compare(0,offName.size(),offName)!=0) {
                cout << "This is not a valid OFF file." << endl;
                return false;
            }

            int i, dummy, num_triangles, num_vertices;
            in >> num_vertices >> num_triangles >> dummy;

            v.resize(num_vertices);
            for (i = 0; i < num_vertices; i++)
                in >> v[i][0] >> v[i][1] >> v[i][2];

            t.resize(num_triangles);

            for (i = 0; i < num_triangles; i++)
                in >> dummy >> t[i].v[0] >> t[i].v[1] >> t[i].v[2];

            in.close();

            // --------- Vector initializations -------------
            out_vertices.clear();
            out_normals.clear();
            gc.clear();
            color_li.clear();

            // vertices array
            vector<Point3d> normals(num_vertices);
            std::fill(normals.begin(), normals.end(), Point3d(0.0f, 0.0f, 0.0f));

            vector<float> triangle_vertices(num_triangles * 9);
            vector<float> triangle_normals(num_triangles * 9);

            // save normals
            vector<int> v_counter(num_vertices);
            std::fill(v_counter.begin(), v_counter.end(), 0); // initialize every vertex normal to (0,0,0)


            // -------------- GAUSSIAN CURVATURE and VERTICES TRIANGLES -----------------
            // find gaussian curvature
            vector<float> triangle_gc(num_triangles * 9);
            color_li.resize(num_triangles * 9);
            vector<float> gc_counter(num_vertices);
            std::fill(gc_counter.begin(), gc_counter.end(), 0);

            // found max and min
            for(int k = 0; k < num_triangles; k++){
                // Update max and min
                set_min(v[t[k].v[0]]);
                set_max(v[t[k].v[0]]);

                set_min(v[t[k].v[1]]);
                set_max(v[t[k].v[1]]);

                set_min(v[t[k].v[2]]);
                set_max(v[t[k].v[2]]);
            }

            // iterate inside triangles and calculates angle_defeact
            for(int k = 0; k < num_triangles; k++){

                Point3d v0 = get_rescaled_value(v[t[k].v[0]]);
                Point3d v1 = get_rescaled_value(v[t[k].v[1]]);
                Point3d v2 = get_rescaled_value(v[t[k].v[2]]);

                // Point3d v0 = v[t[k].v[0]];
                // Point3d v1 = v[t[k].v[1]];
                // Point3d v2 = v[t[k].v[2]];


                //normals
                // for every triangle face compute face normal and normalize it
                Point3d n = (v1-v0)^(v2-v0);
                n.normalize();

                normals[t[k].v[0]] += n;
                v_counter[t[k].v[0]]++; // update counter

                normals[t[k].v[1]] += n;
                v_counter[t[k].v[1]]++; // update counter

                normals[t[k].v[2]] += n;
                v_counter[t[k].v[2]]++; // update counter


                // GAUSSIAN CURVATURE
                // calculate gc for each vertex of triangle
                // VERTEX 1
                // v1 -> v0 -> v2
                Point3d v0v1 = v1 - v0;
                Point3d v0v2 = v2 - v0;
                double sum_angles_1 = v0v1.getAngle(v0v2);

                // VERTEX 2
                // v2 -> v1 -> v0
                Point3d v1v2 = v2 - v1;
                double sum_angles_2 = v1v2.getAngle(-v0v1);

                // VERTEX 3
                // v0 -> v2 -> v1
                double sum_angles_3 = v0v2.getAngle(v1v2);

                // for each triangle-vertex selected add sum_angles
                gc_counter[t[k].v[0]] += sum_angles_1;
                gc_counter[t[k].v[1]] += sum_angles_2;
                gc_counter[t[k].v[2]] += sum_angles_3;


                //LINEAR INTERPOLATION
                color_li[9 * k] = 1.0f; //red x
                color_li[9 * k + 1] = 0.0f; // red y
                color_li[9 * k + 2] = 0.0f; // red z
                color_li[9 * k + 3] = 0.0f;  // green x
                color_li[9 * k + 4] = 1.0f; // green y
                color_li[9 * k + 5] = 0.0f; // green z
                color_li[9 * k + 6] = 0.0f; // blue x
                color_li[9 * k + 7] = 0.0f; // blue y
                color_li[9 * k + 8] = 1.0f; // blue z
            }

                // add everything to triangle gaussian curvature
                for(int k = 0; k < num_triangles; k++){
                    triangle_gc[9*k] = triangle_gc[9*k + 1] = triangle_gc[9*k + 2] = 2 * PI - gc_counter[t[k].v[0]];
                    triangle_gc[9*k + 3] = triangle_gc[9*k + 4] = triangle_gc[9*k + 5] = 2 * PI - gc_counter[t[k].v[1]];
                    triangle_gc[9*k + 6] = triangle_gc[9*k + 7] = triangle_gc[9*k + 8] = 2 * PI - gc_counter[t[k].v[2]];

                    update_statistics_data(triangle_gc[9*k]);
                    update_statistics_data(triangle_gc[9*k + 3]);
                    update_statistics_data(triangle_gc[9*k + 6]);
                }

                finalize_statistics_data();

                for(int k = 0; k < num_triangles; k++){ // update to remove noisy (smoothing)
                    triangle_gc[9*k] = triangle_gc[9*k + 1] = triangle_gc[9*k + 2] = cut_data_gaussian_curvature(2 * PI - gc_counter[t[k].v[0]]);
                    triangle_gc[9*k + 3] = triangle_gc[9*k + 4] = triangle_gc[9*k + 5] = cut_data_gaussian_curvature(2 * PI - gc_counter[t[k].v[1]]);
                    triangle_gc[9*k + 6] = triangle_gc[9*k + 7] = triangle_gc[9*k + 8] = cut_data_gaussian_curvature(2 * PI - gc_counter[t[k].v[2]]);
                }

           // -------------- END GAUSSIAN CURVATURE -----------------

            //normals
            // normalize every vertex normal
            // average of norms of adj triangle of a vertex (sum of triangle norms / number of triangles)
            for(int k = 0; k < num_vertices; k++){
                // normals[k].normalize();
                if(v_counter[k] != 0){
                    normals[k] = normals[k] / v_counter[k];
            }
                normals[k].normalize();
            }

            for (int k = 0; k < num_triangles; k++) {
                // insert vertice values in triangles
                triangle_vertices[9 * k] = get_rescaled_value(v[t[k].v[0]]).x();
                triangle_vertices[9 * k + 1] = get_rescaled_value(v[t[k].v[0]]).y();
                triangle_vertices[9 * k + 2] = get_rescaled_value(v[t[k].v[0]]).z();

                triangle_vertices[9 * k + 3] = get_rescaled_value(v[t[k].v[1]]).x();
                triangle_vertices[9 * k + 4] = get_rescaled_value(v[t[k].v[1]]).y();
                triangle_vertices[9 * k + 5] = get_rescaled_value(v[t[k].v[1]]).z();

                triangle_vertices[9 * k + 6] = get_rescaled_value(v[t[k].v[2]]).x();
                triangle_vertices[9 * k + 7] = get_rescaled_value(v[t[k].v[2]]).y();
                triangle_vertices[9 * k + 8] = get_rescaled_value(v[t[k].v[2]]).z();

                    // insert normal values in triangles
                    triangle_normals[9 * k] = normals[t[k].v[0]].x();
                    triangle_normals[9 * k + 1] = normals[t[k].v[0]].y();
                    triangle_normals[9 * k + 2] = normals[t[k].v[0]].z();

                    triangle_normals[9 * k + 3] = normals[t[k].v[1]].x();
                    triangle_normals[9 * k + 4] = normals[t[k].v[1]].y();
                    triangle_normals[9 * k + 5] = normals[t[k].v[1]].z();

                    triangle_normals[9 * k + 6] = normals[t[k].v[2]].x();
                    triangle_normals[9 * k + 7] = normals[t[k].v[2]].y();
                    triangle_normals[9 * k + 8] = normals[t[k].v[2]].z();
            }

            // output vectors
            //For each vertex of each triangle
            for (unsigned int i = 0; i < triangle_vertices.size(); i++) {
                // get value
                out_vertices.push_back(triangle_vertices[i]);
                out_normals.push_back(triangle_normals[i]);
                gc.push_back(triangle_gc[i]);
            }

            cout << "Object loaded" << endl;
            return true;
        }


int get_number_triangles(){
    return num_triangles;
}

#endif
