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

        bool load (const char * path, vector<float> &out_vertices, vector<Point3d> &out_normals) {
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

            // vertices array
            vector<float> vertices(num_triangles * 18);
            vector<Point3d> normals(num_vertices);
            std::fill(normals.begin(), normals.end(), 0);

            int index = 0;
            // int index_normal = 0;

            // save normals
            vector<int> v_counter(num_vertices);
            std::fill(v_counter.begin(), v_counter.end(), 0); // initialize every vertex normal to (0,0,0)

            // save everything with the color into vertices --- 
            for (int k = 0; k < num_triangles; k++) {    
                Point3d v1 = v[t[k].v[0]];
                Point3d v2 = v[t[k].v[1]];
                Point3d v3 = v[t[k].v[2]];

                // insert values in vertices
                // first vertex
                vertices[index] = v1.x();
                vertices[index + 1] = v1.y();
                vertices[index + 2] = v1.z();

                // second vertex
                vertices[index + 3] = v2.x();
                vertices[index + 4] = v2.y();
                vertices[index + 5] = v2.z();

                // third vertex
                vertices[index + 6] = v3.x();
                vertices[index + 7] = v3.y();
                vertices[index + 8] = v3.z();
  

                index += 9;

                // for every triangle face compute face normal and normalize it
                Point3d n = (v2-v1)^(v3-v1);
                n.normalize();

                // for every vertex of the face add n to the vertex normal
                normals[t[k].v[0]] += n;
                v_counter[t[k].v[0]]++; // update counter

                normals[t[k].v[1]] += n;
                v_counter[t[k].v[1]]++;

                normals[t[k].v[2]] += n;
                v_counter[t[k].v[2]]++;

            }

            // normalize every vertex normal
            // average of norms of adj triangle of a vertex (sum of triangle norms / number of triangles)
            for(int k = 0; k < num_vertices; k++){
                if(v_counter[k] != 0){
                    normals[k] = normals[k] / v_counter[k];
                    normals[k].normalize();
                }
            }


            // find gaussian curvature
            map <int, double> gc_map;  //vertex_j, gc
            for(int i = 0; i < num_vertices; i++){
                int vertex_j = i;
                Point3d vertex_value = Point3d(vertices[i], vertices[i + 1], vertices[i + 2]);

                double sum_angles = 0;

                // for each vertex j, search all triangles adjacent k
                for(int k = 0; k < num_triangles; k++){

                    if(t[k].v[0] == vertex_j){
                        // v1 -> v0 -> v2
                        // 2 vectors v1v0 and v0v2 that means v0-v1 and v2-v0
                        Point3d v0 = vertex_value; //vertex in which I want to find the angle is j
                        Point3d v1 = Point3d(vertices[t[k].v[1]], vertices[t[k].v[1] + 1], vertices[t[k].v[1] + 2]);
                        Point3d v2 = Point3d(vertices[t[k].v[2]], vertices[t[k].v[2] + 1], vertices[t[k].v[2] + 2]);

                        // vectors
                        Point3d v1v0 = Point3d(v0.x() - v1.x(), v0.y() - v1.y(), v0.z() - v1.z());
                        Point3d v0v2 = Point3d(v2.x() - v0.x(), v2.y() - v0.y(), v2.z() - v0.z());

                        double angle = v1v0.getAngle(v0v2);
                        sum_angles += angle;

                    } else if(t[k].v[1] == vertex_j){
                        // v2 -> v1 -> v0
                        // 2 vectors v2v1 and v1v0 that means v1-v2 and v0-v1
                        Point3d v0 = Point3d(vertices[t[k].v[0]], vertices[t[k].v[0] + 1], vertices[t[k].v[0] + 2]);
                        Point3d v1 = vertex_value; //vertex in which I want to find the angle is j
                        Point3d v2 = Point3d(vertices[t[k].v[2]], vertices[t[k].v[2] + 1], vertices[t[k].v[2] + 2]);

                        // vectors
                        Point3d v2v1 = Point3d(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z());
                        Point3d v1v0 = Point3d(v0.x() - v1.x(), v0.y() - v1.y(), v0.z() - v1.z());

                        double angle = v2v1.getAngle(v1v0);
                        sum_angles += angle;

                    }else if(t[k].v[2] == vertex_j){
                        // v0 -> v2 -> v1
                        // 2 vectors v0v2 and v2v1 that means v2-v0 and v1-v2
                        Point3d v0 = Point3d(vertices[t[k].v[0]], vertices[t[k].v[0] + 1], vertices[t[k].v[0] + 2]);
                        Point3d v1 = Point3d(vertices[t[k].v[1]], vertices[t[k].v[1] + 1], vertices[t[k].v[1] + 2]);
                        Point3d v2 = vertex_value; //vertex in which I want to find the angle is j

                        // vectors
                        Point3d v0v2 = Point3d(v2.x() - v0.x(), v2.y() - v0.y(), v2.z() - v0.z());
                        Point3d v2v1 = Point3d(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z());

                        double angle = v0v2.getAngle(v2v1);
                        sum_angles += angle;
                    }
                }

                double gaussian_curvature_j = 2 * PI - sum_angles;
                gc_map.insert(pair <int, double> (vertex_j, gaussian_curvature_j));
            }


            // output vectors ---
            // For each vertex of each triangle
            for (unsigned int i = 0; i < vertices.size(); i++) {
                // get value
                float value = vertices[i];
                out_vertices.push_back(value);
            }

            for (unsigned int i = 0; i < normals.size(); i++) {
                // get value
                Point3d value = normals[i];
                out_normals.push_back(value);
            }

            cout << "Object loaded" << endl;
            return true;
        }

void vecPoint3dToFloat(vector<Point3d> &_vec, vector<float> &_out) {
    _out.clear();
    for (auto i = _vec.begin(); i != _vec.end(); ++i) {
        _out.push_back(i->x());
        _out.push_back(i->y());
        _out.push_back(i->z());
    }
}

#endif
