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

            int index = 0;
            // int index_normal = 0;

            // save normals
            vector<int> v_counter(num_vertices);

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

                // find the norm for the first vertex
                normals[t[k].v[0]] += n;
                v_counter[t[k].v[0]]++;

                normals[t[k].v[1]] += n;
                v_counter[t[k].v[1]]++;

                normals[t[k].v[2]] += n;
                v_counter[t[k].v[2]]++;

            }

            // average of norms of adj triangle of a vertex (sum of triangle norms / number of triangles)
            for(int k = 0; k < num_vertices; k++){
                if(v_counter[k] != 0){
                    normals[k] = normals[k] / v_counter[k];
                    normals[k].normalize();
                }
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
