#ifndef LOADER_H
#define LOADER_H

#include "Point3.h"
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <iterator>

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
int number_triangles;

        bool load (const char * path, vector<float> &out_vertices, vector<float> &out_normals) {
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
            number_triangles = num_triangles;

            // vertices array
            vector<Point3d> vertices(num_vertices);
            vector<Point3d> normals(num_vertices);
            std::fill(normals.begin(), normals.end(), Point3d(0.0f, 0.0f, 0.0f));

            int index = 0;
            // int index_normal = 0;

            vector<float> triangle_vertices(num_triangles * 9);
            vector<float> triangle_normals(num_triangles * 9);

            // save normals
            vector<int> v_counter(num_vertices);
            std::fill(v_counter.begin(), v_counter.end(), 0); // initialize every vertex normal to (0,0,0)

            // save everything with the color into vertices --- 
            for (int k = 0; k < num_triangles; k++) {    

                Point3d v1 = v[t[k].v[0]];
                Point3d v2 = v[t[k].v[1]];
                Point3d v3 = v[t[k].v[2]];

                // for every triangle face compute face normal and normalize it
                Point3d n = (v2-v1)^(v3-v1);
                n.normalize();

                normals[t[k].v[0]] += n;
                v_counter[t[k].v[0]]++; // update counter

                normals[t[k].v[1]] += n;
                v_counter[t[k].v[1]]++; // update counter

                normals[t[k].v[2]] += n;
                v_counter[t[k].v[2]]++; // update counter
            }


            // normalize every vertex normal
            // average of norms of adj triangle of a vertex (sum of triangle norms / number of triangles)
            for(int k = 0; k < num_vertices; k++){
                // normals[k].normalize();
                if(v_counter[k] != 0){ 
                    normals[k] = normals[k] / v_counter[k]; 
               }
                normals[k].normalize();
                std::cout << v[k] << " " << normals[k] << std::endl;
            }

            for (int k = 0; k < num_triangles; k++) {
                // insert vertice values in triangles
                triangle_vertices[9*k] = v[t[k].v[0]].x();
                triangle_vertices[9*k + 1] = v[t[k].v[0]].y();
                triangle_vertices[9*k + 2] = v[t[k].v[0]].z();
                
                triangle_vertices[9*k + 3] = v[t[k].v[1]].x();
                triangle_vertices[9*k + 4] = v[t[k].v[1]].y();
                triangle_vertices[9*k + 5] = v[t[k].v[1]].z();

                triangle_vertices[9*k + 6] = v[t[k].v[2]].x();
                triangle_vertices[9*k + 7] = v[t[k].v[2]].y();
                triangle_vertices[9*k + 8] = v[t[k].v[2]].z();

                // insert normal values in triangles
                triangle_normals[9*k] = normals[t[k].v[0]].x();
                triangle_normals[9*k + 1] = normals[t[k].v[0]].y();
                triangle_normals[9*k + 2] = normals[t[k].v[0]].z();
                
                triangle_normals[9*k + 3] = normals[t[k].v[1]].x();
                triangle_normals[9*k + 4] = normals[t[k].v[1]].y();
                triangle_normals[9*k + 5] = normals[t[k].v[1]].z();

                triangle_normals[9*k + 6] = normals[t[k].v[2]].x();
                triangle_normals[9*k + 7] = normals[t[k].v[2]].y();
                triangle_normals[9*k + 8] = normals[t[k].v[2]].z();
            }



        //     // -------------- GAUSSIAN CURVATURE ----------------- REIMPLEMENT
        //     // find gaussian curvature
        //     map <int, double> gc_map;  //vertex_j, gc
        //     for(int i = 0; i < num_vertices; i++){
        //         int vertex_j = i;
        //         Point3d vertex_value = vertices[i];

        //         double sum_angles = 0;

        //         // for each vertex j, search all triangles adjacent k
        //         for(int k = 0; k < num_triangles; k++){

        //             if(t[k].v[0] == vertex_j){
        //                 // v1 -> v0 -> v2
        //                 // 2 vectors v1v0 and v0v2 that means v0-v1 and v2-v0
        //                 Point3d v0 = vertex_value; //vertex in which I want to find the angle is j
        //                 Point3d v1 = Point3d(vertices[t[k].v[1]], vertices[t[k].v[1] + 1], vertices[t[k].v[1] + 2]);
        //                 Point3d v2 = Point3d(vertices[t[k].v[2]], vertices[t[k].v[2] + 1], vertices[t[k].v[2] + 2]);

        //                 // vectors
        //                 Point3d v1v0 = Point3d(v0.x() - v1.x(), v0.y() - v1.y(), v0.z() - v1.z());
        //                 Point3d v0v2 = Point3d(v2.x() - v0.x(), v2.y() - v0.y(), v2.z() - v0.z());

        //                 double angle = v1v0.getAngle(v0v2);
        //                 sum_angles += angle;

        //             } else if(t[k].v[1] == vertex_j){
        //                 // v2 -> v1 -> v0
        //                 // 2 vectors v2v1 and v1v0 that means v1-v2 and v0-v1
        //                 Point3d v0 = Point3d(vertices[t[k].v[0]], vertices[t[k].v[0] + 1], vertices[t[k].v[0] + 2]);
        //                 Point3d v1 = vertex_value; //vertex in which I want to find the angle is j
        //                 Point3d v2 = Point3d(vertices[t[k].v[2]], vertices[t[k].v[2] + 1], vertices[t[k].v[2] + 2]);

        //                 // vectors
        //                 Point3d v2v1 = Point3d(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z());
        //                 Point3d v1v0 = Point3d(v0.x() - v1.x(), v0.y() - v1.y(), v0.z() - v1.z());

        //                 double angle = v2v1.getAngle(v1v0);
        //                 sum_angles += angle;

        //             }else if(t[k].v[2] == vertex_j){
        //                 // v0 -> v2 -> v1
        //                 // 2 vectors v0v2 and v2v1 that means v2-v0 and v1-v2
        //                 Point3d v0 = Point3d(vertices[t[k].v[0]], vertices[t[k].v[0] + 1], vertices[t[k].v[0] + 2]);
        //                 Point3d v1 = Point3d(vertices[t[k].v[1]], vertices[t[k].v[1] + 1], vertices[t[k].v[1] + 2]);
        //                 Point3d v2 = vertex_value; //vertex in which I want to find the angle is j

        //                 // vectors
        //                 Point3d v0v2 = Point3d(v2.x() - v0.x(), v2.y() - v0.y(), v2.z() - v0.z());
        //                 Point3d v2v1 = Point3d(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z());

        //                 double angle = v0v2.getAngle(v2v1);
        //                 sum_angles += angle;
        //             }
        //         }

        //         double gaussian_curvature_j = 2 * PI - sum_angles;
        //         gc_map.insert(pair <int, double> (vertex_j, gaussian_curvature_j));
        //     }
        //    // -------------- END GAUSSIAN CURVATURE -----------------


            // output vectors
            //For each vertex of each triangle
            out_vertices.clear();
            out_normals.clear();
            for (unsigned int i = 0; i < triangle_vertices.size(); i++) {
                // get value
                out_vertices.push_back(triangle_vertices[i]);
                out_normals.push_back(triangle_normals[i]);
            }


            cout << "Object loaded" << endl;
            return true;
        }


int get_number_triangles(){
    return number_triangles;
}


#endif
