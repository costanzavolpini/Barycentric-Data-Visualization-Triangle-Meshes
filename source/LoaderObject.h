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
static int num_vertices;  // number of vertices in the mesh
// -------------------------

// ----- CROP DATA BETWEEN [-1, 1] -----
// minimum and maximum value found in the mesh
double min_coord = 0.0f;
double max_coord = 0.0f;

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

// vector that contains a new surface area for each vertex x, denoted A_Mixed: for each non-obtuse triangle, we
// use the circumcenter point, and for each obtuse triangle, we use the midpoint
// of the edge opposite to the obtuse angle
vector<float> area_mixed;

vector<float> voronoi_region;
vector<float> normal_curvature_estimation;


// counter obtuse triangle and not-obtuse triangle
int number_obtuse_triangle;
int number_non_obtuse_triangle;
// -------------------------

/**
 * Get distance between 2 points
 */
double get_distance_points(Point3d v0, Point3d v1)
{
    return abs(sqrt(pow((v0[0] - v1[0]), 2) + pow((v0[1] - v1[1]), 2) + pow((v0[2] - v1[2]), 2)));
}

/**
 * Function to insert a struct edge inside edge-map.
 */
void insert_edge(int index_v1, int index_v2, bool isCorrectOrder, Point3d n)
// FIXME: fix mean curvature, but that code seems to work correctly
{
    vector<int> key(2);
    if(isCorrectOrder){
        key[0] = index_v1;
        key[1] = index_v2;
    }
    else{
        key[0] = index_v2;
        key[1] = index_v1;
    }

    std::map<vector<int>, edge>::iterator it;// iterator

    // convention: from smaller to higher index
    it = map_edge.find(key);

    if (it != map_edge.end()) // update information edge struct if exist
    {
        if(isCorrectOrder) // means index_1 < index_2
            it->second.n1 = n;
        else
            it->second.n2 = n;
    }
    else
    {            // create new edge struct
        edge e1; // struct
        e1.length = get_distance_points(v[index_v1], v[index_v2]);
        if (isCorrectOrder) // index_v1 < index_v2
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

        e1.value_mean_curvature = e1.length * (v[e1.index_v2]).getAngle(v[e1.index_v1]) / 2;
        // e1.value_mean_curvature = e1.length * (v[e1.index_v1]).getAngle(v[e1.index_v2]) / 2;
        map_edge[key] = e1;
    }
}

/**
 * Function to get the value of mean curvature of an edge from the edge-map.
 */
float get_mean_curvature(int index_v1, int index_v2, vector<int> key, vector<int> key_2)
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

    // no mean curvature value found
    cout << "ERROR MEAN CURVATURE" << endl;
    exit(-1);
}

/**
 * Function to update the minimum and the maximum value found in coords of an object.
 */
void set_min_max(Point3d current)
{
    min_coord = fmin(fmin(current.x(), current.y()), fmin(current.z(), min_coord));
    max_coord = fmax(fmax(current.x(), current.y()), fmax(current.z(), max_coord));
}


/**
 * Function to set max and min of a mesh
 */
void set_max_min_mesh()
{
    // initialize min_coord and max_coord
    min_coord = fmin(fmin(v[t[0].v[0]].x(), v[t[0].v[0]].y()), v[t[0].v[0]].z());
    max_coord = fmax(fmax(v[t[0].v[0]].x(), v[t[0].v[0]].y()), v[t[0].v[0]].z());

    // set max and min
    for (int k = 0; k < num_triangles; k++)
    {
        // Update max and min
        set_min_max(v[t[k].v[0]]);

        set_min_max(v[t[k].v[1]]);

        set_min_max(v[t[k].v[2]]);
    }
}

/**
 * Function to rescale a coord such that the coords is in a range between -1 and 1.
*/
Point3d get_rescaled_value(Point3d value)
{
    // return value;                                                        // TODO: need to remove afer
    return interval / (max_coord - min_coord) * (value - max_coord) + 1; //1 is the max of interval
}

/**
 * Find area of triangle using Heron's formula.
 * s = (a + b + c) / 2
 * A = sqrt(s (s - a) (s - b) (s-c))
*/
double get_area_triangle(int index_triangle)
{
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
double get_cotangent(double angle)
{
    return cos(angle) / sin(angle);
}

/**
 * Function to get Voronoi region of vertex P in triangle [P, Q, R].
 * See paper http://www.geometry.caltech.edu/pubs/DMSB_III.pdf (section 3.3)
 */
double get_voronoi_region_triangle(int P_index, int Q_index, int R_index, float Q_angle, float R_angle)
{
    Point3d P = get_rescaled_value(v[P_index]);
    Point3d Q = get_rescaled_value(v[Q_index]);
    Point3d R = get_rescaled_value(v[R_index]);
    double first_part = pow(get_distance_points(P, R), 2) * get_cotangent(Q_angle);
    double second_part = pow(get_distance_points(P, Q), 2) * get_cotangent(R_angle);
    return (first_part + second_part) / 8;
}

/**
 * Check if an angle is obtuse (radians)
 */
bool is_obtuse_angle(float angle){
    return angle > M_PI/2 && angle < M_PI;
}

/**
 * Calculate Area mixed, given the index of the triangle, index of the  vertex and 3 angles of the triangle.
 * This function will be called for each triangle T from the 1-ring neighborhood of x (current angle)
*/
void calculate_A_mixed(int index_triangle, int index_vertex, int index_vertex_other, int index_vertex_other1, float current_angle, float other_angle, float other_angle_1)
{
    if (!is_obtuse_angle(current_angle) && !is_obtuse_angle(other_angle) && !is_obtuse_angle(other_angle_1)) // Triangle is not obtuse -> Voronoi-safe
    {
        // Voronoi region of x in T
        area_mixed[index_vertex] += get_voronoi_region_triangle(index_vertex, index_vertex_other, index_vertex_other1, other_angle, other_angle_1);
    }
    else // Voronoi inappropriate
    {
        if (is_obtuse_angle(current_angle)) //obtuse angle
            area_mixed[index_vertex] += get_area_triangle(index_triangle) / 2;
        else // not-obtuse triangle
            area_mixed[index_vertex] += get_area_triangle(index_triangle) / 4;
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
bool read_off_file(const char *path)
{
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
bool load(const char *path, vector<float> &out_vertices, vector<float> &out_normals, vector<float> &out_gc, vector<float> &out_mc, vector<float> &out_mc_vertex, vector<float> &gc_vertex_size, vector<float> &mc_edge_size)
{
    // --------------------- Read file -----------------------------
    if (!read_off_file(path))
        return false;

    // size out_vertices, out_normals, out_gc, out_mc = num_triangles * 9

    number_obtuse_triangle = 0;
    number_non_obtuse_triangle = 0;

    // ------- VECTOR INITIALIZATION -------
    // vector that contains Point3d normal
    vector<Point3d> normals(num_vertices);
    std::fill(normals.begin(), normals.end(), Point3d(0.0f, 0.0f, 0.0f));

    vector<int> v_counter(num_vertices);              // vector to count faces for each vertex to calculate normals
    std::fill(v_counter.begin(), v_counter.end(), 0); // initialize every vertex normal to (0,0,0)
    // ----

    // -- initialize Gaussian curvature vectors --
    // vector containing all triangle gaussian value for each vertex
    // for compatibility values are saved 3 times for each vertex
    vector<float> value_angle_defeact_sum(num_vertices); // vector containing current sum of partial gaussian curvature per vertex
    std::fill(value_angle_defeact_sum.begin(), value_angle_defeact_sum.end(), 0);

    area_mixed.resize(num_vertices); // vector containing area_mixed (obtuse and not-obtuse triangle)
    std::fill(area_mixed.begin(), area_mixed.end(), 0);
    // ---- end Gaussian curvature vectors initialization ----

    set_max_min_mesh(); // find min value and max value of a mesh (in order to rescale values correctly)

    // -- initialize mean curvature per vertex vectors --
    voronoi_region.resize(num_vertices);
    std::fill(voronoi_region.begin(), voronoi_region.end(), 0);

    normal_curvature_estimation.resize(num_vertices);
    std::fill(normal_curvature_estimation.begin(), normal_curvature_estimation.end(), 0);
    // ---- end mean curvature vectors per vertex initialization ----

    // iterate inside triangles and calculates angle_defeact
    for (int k = 0; k < num_triangles; k++)
    {
        Point3d v0 = get_rescaled_value(v[t[k].v[0]]);
        Point3d v1 = get_rescaled_value(v[t[k].v[1]]);
        Point3d v2 = get_rescaled_value(v[t[k].v[2]]);

        // -- normals ---
        // for every triangle face compute face normal and normalize it
        Point3d n = (v1 - v0) ^ (v2 - v0);
        n.normalize();

        // add value to normals vector at the vector
        normals[t[k].v[0]] += n;
        normals[t[k].v[1]] += n;
        normals[t[k].v[2]] += n;

        v_counter[t[k].v[0]]++; // update counter for normals
        v_counter[t[k].v[1]]++; // update counter for normals
        v_counter[t[k].v[2]]++; // update counter for normals

        // -------------- MEAN CURVATURE EDGE -------------- TODO: check the code

        // fill map edges
        int index_v1 = t[k].v[0];
        int index_v2 = t[k].v[1];
        int index_v3 = t[k].v[2];

        bool isCorrectedOrder = false;

        if(index_v1 < index_v2)
            isCorrectedOrder = true;

        insert_edge(index_v1, index_v2, isCorrectedOrder, n);

        isCorrectedOrder = false;

        if(index_v3 < index_v1)
            isCorrectedOrder = true;

        insert_edge(index_v3, index_v1, isCorrectedOrder, n);

        isCorrectedOrder = false;

        if(index_v2 < index_v3)
            isCorrectedOrder = true;

        insert_edge(index_v2, index_v3, isCorrectedOrder, n);

        // -------------- end mean curvature --------------

        // -------------- GAUSSIAN CURVATURE --------------
        // calculate angle defeact for each vertex of triangle
        // vertex 1
        // v1 -> v0 -> v2
        Point3d v0v1 = v1 - v0;
        Point3d v0v2 = v2 - v0;
        v0v1.normalize();
        v0v2.normalize();
        double angle_v1v0v2 = v0v1.getAngle(v0v2);

        // vertex 2
        // v2 -> v1 -> v0
        Point3d v1v2 = v2 - v1;
        v1v2.normalize();
        double angle_v2v1v0 = v1v2.getAngle(-v0v1); // -v0v1 = v1v0

        // vertex 3
        // v0 -> v2 -> v1
        double angle_v0v2v1 = (-v0v2).getAngle(-v1v2); // same as v0v2.getAngle(v1v2)

        // for each vertex of the triangle updated its value of gc (sum_(j=1)^(#faces around this vertex) vertex_j)
        value_angle_defeact_sum[t[k].v[0]] += angle_v1v0v2;
        value_angle_defeact_sum[t[k].v[1]] += angle_v2v1v0;
        value_angle_defeact_sum[t[k].v[2]] += angle_v0v2v1;

        // find A_mixed (obtuse and not obtuse triangle)
        calculate_A_mixed(k, index_v1, index_v2, index_v3, angle_v1v0v2, angle_v2v1v0, angle_v0v2v1);
        calculate_A_mixed(k, index_v2, index_v1, index_v3, angle_v2v1v0, angle_v1v0v2, angle_v0v2v1);
        calculate_A_mixed(k, index_v3, index_v1, index_v2, angle_v0v2v1, angle_v1v0v2, angle_v2v1v0);

        // if (!is_obtuse_angle(angle_v1v0v2) && !is_obtuse_angle(angle_v2v1v0) && !is_obtuse_angle(angle_v0v2v1)) // Triangle is not obtuse
        //     number_non_obtuse_triangle++;
        // else // triangle obtuse
        //     number_obtuse_triangle++;

        // -------------- end mean curvature edge --------------

        // -------------- MEAN CURVATURE VERTEX --------------
        // array of A_mixed saved for each vertex
        voronoi_region[index_v1] += get_voronoi_region_triangle(index_v1, index_v2, index_v3, angle_v2v1v0, angle_v0v2v1);
        // voronoi_region[index_v1] += get_voronoi_region_triangle(index_v1, index_v3, index_v2, angle_v0v2v1, angle_v2v1v0);
        // TODO: maybe add also voronoi region between v1 and v3 and so on

        voronoi_region[index_v2] += get_voronoi_region_triangle(index_v2, index_v1, index_v3, angle_v1v0v2, angle_v0v2v1);
        voronoi_region[index_v3] += get_voronoi_region_triangle(index_v3, index_v1, index_v2, angle_v1v0v2, angle_v2v1v0);


        normal_curvature_estimation[index_v1] += 2 * (((v[index_v1] - v[index_v2]) * n)/((v[index_v1] - v[index_v2]).squaredNorm()));
        normal_curvature_estimation[index_v1] += 2 * (((v[index_v1] - v[index_v3]) * n)/((v[index_v1] - v[index_v3]).squaredNorm()));

        normal_curvature_estimation[index_v2] += 2 * (((v[index_v2] - v[index_v1]) * n)/((v[index_v2] - v[index_v1]).squaredNorm()));
        normal_curvature_estimation[index_v2] += 2 * (((v[index_v2] - v[index_v3]) * n)/((v[index_v2] - v[index_v3]).squaredNorm()));

        normal_curvature_estimation[index_v3] += 2 * (((v[index_v3] - v[index_v2]) * n)/((v[index_v3] - v[index_v2]).squaredNorm()));
        normal_curvature_estimation[index_v3] += 2 * (((v[index_v3] - v[index_v1]) * n)/((v[index_v3] - v[index_v1]).squaredNorm()));
    }

    // fill out_gc vector
    // k_G = (2PI - sum_angle_defeact)/A_mixed
    for (int k = 0; k < num_triangles; k++)
    {
        // vertex 0
        out_gc.push_back(((2 * M_PI) - value_angle_defeact_sum[t[k].v[0]]) / area_mixed[t[k].v[0]]);
        out_gc.push_back(((2 * M_PI) - value_angle_defeact_sum[t[k].v[0]]) / area_mixed[t[k].v[0]]);
        out_gc.push_back(((2 * M_PI) - value_angle_defeact_sum[t[k].v[0]]) / area_mixed[t[k].v[0]]);

        // vertex 1
        out_gc.push_back(((2 * M_PI) - value_angle_defeact_sum[t[k].v[1]]) / area_mixed[t[k].v[1]]);
        out_gc.push_back(((2 * M_PI) - value_angle_defeact_sum[t[k].v[1]]) / area_mixed[t[k].v[1]]);
        out_gc.push_back(((2 * M_PI) - value_angle_defeact_sum[t[k].v[1]]) / area_mixed[t[k].v[1]]);

        // vertex 2
        out_gc.push_back(((2 * M_PI) - value_angle_defeact_sum[t[k].v[2]]) / area_mixed[t[k].v[2]]);
        out_gc.push_back(((2 * M_PI) - value_angle_defeact_sum[t[k].v[2]]) / area_mixed[t[k].v[2]]);
        out_gc.push_back(((2 * M_PI) - value_angle_defeact_sum[t[k].v[2]]) / area_mixed[t[k].v[2]]);

        // -------------- end Gaussian curvature --------------

        // -------------- CALCULATE MEAN CURVATURE --------------
        // v1 -> v3v2 {index_v2, index_v3};
        // v2 -> v1v3 {index_v3, index_v1};
        // v3 -> v2v1 {index_v1, index_v2};

        float mc_1, mc_2, mc_3;

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

        // out_mc vector values
        out_mc.push_back(mc_1);
        out_mc.push_back(mc_1);
        out_mc.push_back(mc_1);

        out_mc.push_back(mc_2);
        out_mc.push_back(mc_2);
        out_mc.push_back(mc_2);

        out_mc.push_back(mc_3);
        out_mc.push_back(mc_3);
        out_mc.push_back(mc_3);
        // -------------- end mean curvature --------------

        out_mc_vertex.push_back( 1/area_mixed[t[k].v[0]] * 1/8 * voronoi_region[t[k].v[0]] * normal_curvature_estimation[t[k].v[0]]);
        out_mc_vertex.push_back( 1/area_mixed[t[k].v[1]] * 1/8 * voronoi_region[t[k].v[1]] * normal_curvature_estimation[t[k].v[1]]);
        out_mc_vertex.push_back( 1/area_mixed[t[k].v[2]] * 1/8 * voronoi_region[t[k].v[2]] * normal_curvature_estimation[t[k].v[2]]);
                // voronoi_region normal_curvature_estimation

    }
    // ofstream file_output;
    // string path_name = path;
    // file_output.open (path_name + ".txt");
    for (int k = 0; k < num_vertices; k++){

        // gc_vertex_size lenght = vertices
        gc_vertex_size.push_back(((2 * M_PI) - value_angle_defeact_sum[k]) / area_mixed[k]);

         // write in a file all values of Gaussian curvature
        // file_output << ((2 * M_PI) - value_angle_defeact_sum[k]) / area_mixed[k] << "\n";
    }

    mc_edge_size.reserve(mc_edge_size.size());
    for(auto const& e : map_edge){
        mc_edge_size.push_back(e.second.value_mean_curvature);
    }
    // file_output.close();

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

    // output vectors
    //For each vertex of each triangle
    for (unsigned int k = 0; k < num_triangles; k++)
    {
        // insert vertices values in out_vertices
        out_vertices.push_back(get_rescaled_value(v[t[k].v[0]]).x());
        out_vertices.push_back(get_rescaled_value(v[t[k].v[0]]).y());
        out_vertices.push_back(get_rescaled_value(v[t[k].v[0]]).z());

        out_vertices.push_back(get_rescaled_value(v[t[k].v[1]]).x());
        out_vertices.push_back(get_rescaled_value(v[t[k].v[1]]).y());
        out_vertices.push_back(get_rescaled_value(v[t[k].v[1]]).z());

        out_vertices.push_back(get_rescaled_value(v[t[k].v[2]]).x());
        out_vertices.push_back(get_rescaled_value(v[t[k].v[2]]).y());
        out_vertices.push_back(get_rescaled_value(v[t[k].v[2]]).z());

        // insert normals in out_normals
        out_normals.push_back(normals[t[k].v[0]].x());
        out_normals.push_back(normals[t[k].v[0]].y());
        out_normals.push_back(normals[t[k].v[0]].z());

        out_normals.push_back(normals[t[k].v[1]].x());
        out_normals.push_back(normals[t[k].v[1]].y());
        out_normals.push_back(normals[t[k].v[1]].z());

        out_normals.push_back(normals[t[k].v[2]].x());
        out_normals.push_back(normals[t[k].v[2]].y());
        out_normals.push_back(normals[t[k].v[2]].z());
    }

    cout << path << " "<< number_obtuse_triangle << ", " << number_non_obtuse_triangle << endl;
    cout << "Object loaded" << endl;

    // ------- clear vectors -------
    normals.clear();
    normals.shrink_to_fit();

    v_counter.clear();
    v_counter.shrink_to_fit();

    value_angle_defeact_sum.clear();
    value_angle_defeact_sum.shrink_to_fit();

    // ----------------------------

    return true;
}

int get_number_triangles()
{
    return num_triangles;
}

#endif
