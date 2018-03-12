#ifndef LOADER_H
#define LOADER_H

#include "Point3.h"
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;


/***************************************************************************
LoaderObject.h
Comment:  This file load the mesh using an .OFF file.
***************************************************************************/

// List of vertices and triangles
vector<Point3d> v;
struct Triangle { int v[3]; };
vector<Triangle> t;

vector<Point3d> v_norm; 

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

            v_norm.resize(num_vertices);

            // vertices array
            vector<float> vertices(num_triangles * 18);

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

                //color
                vertices[index + 3] = 1.0f; // v1 red
                vertices[index + 4] = 0.0f;
                vertices[index + 5] = 0.0f;

                // second vertex
                vertices[index + 6] = v2.x();
                vertices[index + 7] = v2.y();
                vertices[index + 8] = v2.z();

                //color
                vertices[index + 9] = 0.0f;
                vertices[index + 10] = 1.0f; // v2 green
                vertices[index + 11] = 0.0f;

                // third vertex
                vertices[index + 12] = v3.x();
                vertices[index + 13] = v3.y();
                vertices[index + 14] = v3.z();

                //color
                vertices[index + 15] = 0.0f;
                vertices[index + 16] = 0.0f;
                vertices[index + 17] = 1.0f; // v3 blue    

                index += 18;

                // normal of a triangle
                Point3d n = (v2-v1)^(v3-v1);
                n.normalize();
            }

            vector<float> temp_vertices;

            // out_vertices.reserve(num_triangles * 18);
            // For each vertex of each triangle
            for (unsigned int i = 0; i < vertices.size(); i++) {
                // get value
                float value = vertices[i];

                out_vertices.push_back(value);
            }

            cout << "Object loaded" << endl;
            return true;
        }

#endif
