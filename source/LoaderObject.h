#ifndef LOADER_H
#define LOADER_H

#include "Point3.h"
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <iterator>
#include "glm/ext.hpp"
#include <map>

#define _USE_MATH_DEFINES
#include <math.h> /* fmin, sqrt */

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#ifndef M_PIl
#define M_PIl (3.14159265358979323846264338327950288)
#endif

using namespace std;

/***************************************************************************
LoaderObject.h
Comment:  This file load the mesh using an .OFF file.
***************************************************************************/

// ----- SET UP DATA AND VERTICES -----
vector<Point3d> v; // vector of vertices

struct Triangle
{
    int v[3];
};
vector<Triangle> t; // vector of triangles

static int num_triangles; // number of triangles in the mesh
static int num_vertices ; // number of vertices in the mesh
// -------------------------

// ----- CROP DATA BETWEEN [-1, 1] -----
// minimum and maximum value found in the mesh
double min_coord = 0.0f;
double max_coord = 0.0f;
bool first_min_max = true;

// interval of loaded mesh will be from -1 to 1
int interval = 2; // max - min = 1 - (-1)
// -------------------------

// ----- MEAN CURVATURE -----
struct edge
{
    double length;
    int index_v1;
    int index_v2;
    Point3d n1;
    Point3d n2;
    double value_mean_curvature;
};

// map for struct edge
std::map<vector<int>, edge> map_edge; // key: [index_v1, index_v2]    val: struct edge

vector<float> area_mixed;
// -------------------------

/**
 * Get distance between 2 points
 */
double get_distance_points(Point3d v0, Point3d v1){
    return abs(sqrt(pow((v0[0] -v1[0]), 2) + pow((v0[1] -v1[1]), 2) + pow((v0[2] -v1[2]), 2)));
}

/**
 * Function to insert a struct edge inside edge-map.
 */
void insert_edge(int index_v1, int index_v2, vector<int> key, vector<int> key_2, Point3d n)
{
    std::map<vector<int>, edge>::iterator it;         // iterator
    std::map<vector<int>, edge>::iterator it_reverse; // iterator for reverse vertices index

    // convention: from smaller to higher index
    it = map_edge.find(key);
    it_reverse = map_edge.find(key_2);

    if (it != map_edge.end()) // update information edge struct if exist
    {
        if (index_v1 < index_v2)
            it->second.n1 = n;
        else
            it->second.n2 = n;
    }
    else if (it_reverse != map_edge.end())
    { // update information edge struct
        if (index_v1 < index_v2)
            it_reverse->second.n1 = n;
        else
            it_reverse->second.n2 = n;
    }
    else
    {            // create new edge struct
        edge e1; // struct
        e1.length = get_distance_points(v[index_v1], v[index_v2]);
        if (index_v1 < index_v2)
        {
            // correct order
            e1.index_v1 = index_v1;
            e1.index_v2 = index_v2;
            e1.n1 = n;
        }
        else
        { // reverse case
            e1.index_v1 = index_v2;
            e1.index_v2 = index_v1;
            e1.n2 = n;
        }

        e1.value_mean_curvature = e1.length * (v[e1.index_v1]).getAngle(v[e1.index_v2]) / 2;
        map_edge[key] = e1;
    }
}

/**
 * Function to get the value of mean curvature of an edge from the edge-map.
 */
int get_mean_curvature(int index_v1, int index_v2, vector<int> key, vector<int> key_2)
{
    std::map<vector<int>, edge>::iterator it;         // iterator
    std::map<vector<int>, edge>::iterator it_reverse; // iterator for reverse vertices index

    // convention: from smaller to higher index to avoid to count double
    it = map_edge.find(key);
    it_reverse = map_edge.find(key_2);

    if (it != map_edge.end())
    {
        return it->second.value_mean_curvature;
    }
    else if (it_reverse != map_edge.end())
    {
        return it_reverse->second.value_mean_curvature;
    }

    // cout << key[0] << " " <<  key[1] << endl;
    // no mean curvature value found
    cout << "ERROR MEAN CURVATURE" << endl;
    exit(-1);
}

/**
 * Function to update the minimum and the maximum value found in coords of an object.
 */
void set_min_max(Point3d current)
{
    if (first_min_max)
    {
        min_coord = fmin(fmin(current.x(), current.y()), current.z());
        max_coord = fmax(fmax(current.x(), current.y()), current.z());
        first_min_max = false;
        return;
    }
    min_coord = fmin(fmin(current.x(), current.y()), fmin(current.z(), min_coord));
    max_coord = fmax(fmax(current.x(), current.y()), fmax(current.z(), max_coord));
}

/**
 * Function to get the maximum value found in coords of an object.
*/
double get_max_coord()
{
    return max_coord;
}

/**
 * Function to get the minimum value found in coords of an object.
*/
double get_min_coord()
{
    return min_coord;
}

/**
 * Function to rescale a coord such that the coords is in a range between -1 and 1.
*/
Point3d get_rescaled_value(Point3d value)
{
    return value; // need to remove afer
    return interval / (max_coord - min_coord) * (value - max_coord) + 1; //1 is the max of interval
}


/**
 * Find area of triangle using Heron's formula.
 * s = (a + b + c) / 2
 * A = sqrt(s (s - a) (s - b) (s-c))
*/
double get_area_triangle(int index_triangle){
    Point3d v0 = get_rescaled_value(v[t[index_triangle].v[0]]);
    Point3d v1 = get_rescaled_value(v[t[index_triangle].v[1]]);
    Point3d v2 = get_rescaled_value(v[t[index_triangle].v[2]]);

    double edge0 = get_distance_points(v0, v1);
    double edge1 = get_distance_points(v0, v2);
    double edge2 = get_distance_points(v1, v2);

    double s = (edge0 + edge1 + edge2) / 2;
    return sqrt(s * (s - edge0) * (s - edge1) * (s - edge2));
}

/**
 * Function to get the cotangent of an angle
 */
double get_cotangent(double angle){
    return tan(M_PI_2 - angle);
}

/**
 * Function to get Voronoi region of vertex P in triangle [P, Q, R]
 */
double get_voronoi_region_triangle(int index_triangle, int P_index, int Q_index, int R_index, float Q_angle, float R_angle){
    Point3d P = get_rescaled_value(v[t[index_triangle].v[P_index]]);
    Point3d Q = get_rescaled_value(v[t[index_triangle].v[Q_index]]);
    Point3d R = get_rescaled_value(v[t[index_triangle].v[R_index]]);
    double first_part = pow(get_distance_points(P, R), 2) * get_cotangent(Q_angle);
    double second_part = pow(get_distance_points(P, Q), 2) * get_cotangent(R_angle);
    return 1/8 * (first_part + second_part);

}

/**
 * Calculate Area mixed, given the index of the triangle, index of the  vertex and 3 angles of the triangle
*/
void calculate_A_mixed(int index_triangle, int index_vertex, int index_vertex_other, int index_vertex_other1, float current_angle, float other_angle, float other_angle_1){
    if(current_angle <= 90 && other_angle <= 90 && other_angle_1 <= 0) // Triangle is not obtuse -> Voronoi-safe
    {
        // Voronoi region of x in T
        area_mixed[index_vertex] += get_voronoi_region_triangle(index_triangle, index_vertex, index_vertex_other, index_vertex_other1, other_angle, other_angle_1);
    } else  // Voronoi inappropriate
    {
        if (current_angle > 90) //obtuse angle
            area_mixed[index_vertex] += get_area_triangle(index_triangle)/2;
        else // not-obtuse triangle
            area_mixed[index_vertex] += get_area_triangle(index_triangle)/4;
    }
}

/**
 * Function to clean allocated memory in order to load correctly different meshes.
 */
void clean()
{
    t.clear();
    t.shrink_to_fit();
    v.clear();
    v.shrink_to_fit();
}

/**
 * Function to read the file.off and fill vectors
*/
bool read_off_file(const char *path){
    ifstream in(path);
    if (!in)
    {
        cout << "\nError reading file." << endl;
        return false;
    }

    string s;
    string offName = "OFF";
    getline(in, s);

    if (s.compare(0, offName.size(), offName) != 0)
    {
        cout << "This is not a valid OFF file." << endl;
        return false;
    }

    int i, dummy;
    in >> num_vertices >> num_triangles >> dummy;

    v.resize(num_vertices);
    for (i = 0; i < num_vertices; i++)
        in >> v[i][0] >> v[i][1] >> v[i][2];

    t.resize(num_triangles);

    for (i = 0; i < num_triangles; i++)
        in >> dummy >> t[i].v[0] >> t[i].v[1] >> t[i].v[2];

    in.close();
    return true;

}

/**
 * Function to load the mesh, find Gaussian Curvature, Mean Curvature...etc.
*/
bool load(const char *path, vector<float> &out_vertices, vector<float> &out_normals, vector<float> &gc, vector<float> &mc, vector<float> &color_li)
{
    // --------------------- Read file -----------------------------
    if(!read_off_file(path))
        return false;

    first_min_max = true;

    // --------- Vector initializations -------------
    // vertices array
    vector<Point3d> normals(num_vertices);
    std::fill(normals.begin(), normals.end(), Point3d(0.0f, 0.0f, 0.0f));

    vector<float> triangle_vertices(num_triangles * 9);

    vector<float> triangle_normals(num_triangles * 9);

    // save normals
    vector<int> v_counter(num_vertices);
    std::fill(v_counter.begin(), v_counter.end(), 0); // initialize every vertex normal to (0,0,0)

    // -------------- GAUSSIAN CURVATURE, MEAN CURVATURE and VERTICES TRIANGLES -----------------
    // find gaussian curvature
    vector<float> triangle_gc(num_triangles * 9);

    color_li.resize(num_triangles * 9);
    vector<float> value_gc_summed(num_vertices);
    std::fill(value_gc_summed.begin(), value_gc_summed.end(), 0);

    area_mixed.resize(num_vertices);
    std::fill(area_mixed.begin(), area_mixed.end(), 0);

    // mean curvature
    vector<float> triangle_mc(num_triangles * 9);

    // found max and min
    for (int k = 0; k < num_triangles; k++)
    {
        // Update max and min
        set_min_max(v[t[k].v[0]]);

        set_min_max(v[t[k].v[1]]);

        set_min_max(v[t[k].v[2]]);
    }

    // iterate inside triangles and calculates angle_defeact
    for (int k = 0; k < num_triangles; k++)
    {

        Point3d v0 = get_rescaled_value(v[t[k].v[0]]);
        Point3d v1 = get_rescaled_value(v[t[k].v[1]]);
        Point3d v2 = get_rescaled_value(v[t[k].v[2]]);

        cout << "TRIANGLE " << endl;
        cout << "v0 " << v0 << endl;
        cout  << "v1 " <<  v1 << endl;
        cout  << "v2 " <<  v2 << endl;

        //normals
        // for every triangle face compute face normal and normalize it
        Point3d n = (v1 - v0) ^ (v2 - v0);
        n.normalize();

        normals[t[k].v[0]] += n;

        normals[t[k].v[1]] += n;

        normals[t[k].v[2]] += n;


        v_counter[t[k].v[0]]++; // update counter for normals
        v_counter[t[k].v[1]]++; // update counter
        v_counter[t[k].v[2]]++; // update counter

        // -------------- MEAN CURVATURE --------------

        // fill map edges
        int index_v1 = t[k].v[0];
        int index_v2 = t[k].v[1];
        int index_v3 = t[k].v[2];

        // index vertices for edges
        vector<int> v2v1 = {index_v1, index_v2};
        vector<int> v1v3 = {index_v3, index_v1};
        vector<int> v3v2 = {index_v2, index_v3};

        vector<int> v2v1_reverse = {index_v2, index_v1};
        vector<int> v1v3_reverse = {index_v1, index_v3};
        vector<int> v3v2_reverse = {index_v3, index_v2};

        insert_edge(index_v1, index_v2, v2v1, v2v1_reverse, n);
        insert_edge(index_v3, index_v1, v1v3, v1v3_reverse, n);
        insert_edge(index_v2, index_v3, v3v2, v3v2_reverse, n);

        // -------------- end mean curvature --------------

        // GAUSSIAN CURVATURE
        // calculate gc for each vertex of triangle
        // VERTEX 1
        // v1 -> v0 -> v2
        Point3d v0v1 = v1 - v0;
        Point3d v0v2 = v2 - v0;
        double angle_1 = v0v1.getAngle(v0v2);

        cout << "angle_1 with v0v2 " << angle_1 << endl;

        // VERTEX 2
        // v2 -> v1 -> v0
        Point3d v1v2 = v2 - v1;
        double angle_2 = v1v2.getAngle(-v0v1);

        cout << "angle_2 with -v0v1 " << angle_2 << endl;

        // VERTEX 3
        // v0 -> v2 -> v1
        double angle_3 = v0v2.getAngle(v1v2);

        cout << "angle_3 with v0v2 with v1v2 " << angle_3 << endl;

        cout << "end!! --------- " << endl;

        // for each vertex of the triangle updated its value of gc (sum_(j=1)^(#faces around this vertex) vertex_j)
        value_gc_summed[t[k].v[0]] += angle_1;
        value_gc_summed[t[k].v[1]] += angle_2;
        value_gc_summed[t[k].v[2]] += angle_3;

        calculate_A_mixed(k, index_v1, index_v2, index_v3, angle_1, angle_2, angle_3);
        calculate_A_mixed(k, index_v2, index_v1, index_v3, angle_2, angle_1, angle_3);
        calculate_A_mixed(k, index_v3, index_v1, index_v2, angle_3, angle_1, angle_2);

        // -------- LINEAR INTERPOLATION ----------
        color_li[9 * k] = 1.0f;     //red x
        color_li[9 * k + 1] = 0.0f; // red y
        color_li[9 * k + 2] = 0.0f; // red z
        color_li[9 * k + 3] = 0.0f; // green x
        color_li[9 * k + 4] = 1.0f; // green y
        color_li[9 * k + 5] = 0.0f; // green z
        color_li[9 * k + 6] = 0.0f; // blue x
        color_li[9 * k + 7] = 0.0f; // blue y
        color_li[9 * k + 8] = 1.0f; // blue z
    }

    // add everything to triangle gaussian curvature
    for (int k = 0; k < num_triangles; k++)
    {
        triangle_gc[9 * k]  = (2 * M_PI - value_gc_summed[t[k].v[0]]) / area_mixed[t[k].v[0]]; //vertex 0
        triangle_gc[9 * k + 3] = (2 * M_PI - value_gc_summed[t[k].v[1]]) / area_mixed[t[k].v[1]]; //vertex 1
        triangle_gc[9 * k + 6] = (2 * M_PI - value_gc_summed[t[k].v[2]]) / area_mixed[t[k].v[2]]; //vertex 2

        cout << "1: " << 2 * M_PI - value_gc_summed[t[k].v[0]] << endl;
        cout << "2: " << 2 * M_PI - value_gc_summed[t[k].v[1]] << endl;
        cout << "3: " << 2 * M_PI - value_gc_summed[t[k].v[2]] << endl;

        triangle_gc[9 * k + 1] = triangle_gc[9 * k + 2] = triangle_gc[9 * k + 4] = triangle_gc[9 * k + 5] = triangle_gc[9 * k + 7] = triangle_gc[9 * k + 8] = 0.0f;
    }

    // -------------- END GAUSSIAN CURVATURE -----------------

    // normals
    // normalize every vertex normal
    // average of norms of adj triangle of a vertex (sum of triangle norms / number of triangles)
    for (int k = 0; k < num_vertices; k++)
    {
        if (v_counter[k] != 0)
        {
            normals[k] = normals[k] / v_counter[k];
        }
        normals[k].normalize();
    }

    for (int k = 0; k < num_triangles; k++)
    {
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

        // -------- insert mean curvature --------------
        // v1 -> v3v2 {index_v2, index_v3};
        // v2 -> v1v3 {index_v3, index_v1};
        // v3 -> v2v1 {index_v1, index_v2};

        int mc_1, mc_2, mc_3;

        int index_v1 = t[k].v[0];
        int index_v2 = t[k].v[1];
        int index_v3 = t[k].v[2];

        vector<int> v2v1 = {index_v1, index_v2};
        vector<int> v1v3 = {index_v3, index_v1};
        vector<int> v3v2 = {index_v2, index_v3};

        vector<int> v2v1_reverse = {index_v2, index_v1};
        vector<int> v1v3_reverse = {index_v1, index_v3};
        vector<int> v3v2_reverse = {index_v3, index_v2};

        mc_1 = get_mean_curvature(index_v2, index_v3, v3v2, v3v2_reverse);
        mc_2 = get_mean_curvature(index_v3, index_v1, v1v3, v1v3_reverse);
        mc_3 = get_mean_curvature(index_v1, index_v2, v2v1, v2v1_reverse);

        // std::cout << "mymap.size() is " << map_edge.size() << '\n';

        triangle_mc[9 * k] = triangle_mc[9 * k + 1] = triangle_mc[9 * k + 2] = mc_1;
        triangle_mc[9 * k + 3] = triangle_mc[9 * k + 4] = triangle_mc[9 * k + 5] = mc_2;
        triangle_mc[9 * k + 6] = triangle_mc[9 * k + 7] = triangle_mc[9 * k + 8] = mc_3;
        // create vector of mean curvature
    }

    // output vectors
    //For each vertex of each triangle
    for (unsigned int i = 0; i < triangle_vertices.size(); i++)
    {
        // get value
        out_vertices.push_back(triangle_vertices[i]);
        out_normals.push_back(triangle_normals[i]);
        gc.push_back(triangle_gc[i]);
        mc.push_back(triangle_mc[i]);
    }

    cout << "Object loaded" << endl;

    // ------- clear vectors -------
    normals.clear();
    normals.shrink_to_fit();

    triangle_normals.clear();
    triangle_normals.shrink_to_fit();

    triangle_vertices.clear();
    triangle_vertices.shrink_to_fit();

    v_counter.clear();
    v_counter.shrink_to_fit();

    triangle_gc.clear();
    triangle_gc.shrink_to_fit();

    value_gc_summed.clear();
    value_gc_summed.shrink_to_fit();

    triangle_mc.clear();
    triangle_mc.shrink_to_fit();
    // ----------------------------
    return true;
}

int get_number_triangles()
{
    return num_triangles;
}

#endif
