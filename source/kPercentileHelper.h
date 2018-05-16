#ifndef KPERCENTILEHELPER_H
#define KPERCENTILEHELPER_H

#include "Point3.h"
#include "Base.h"
#include <math.h>

using namespace std;

/***************************************************************************
GaussianCurvatureHelper.h
Comment:  This file contains all Statistics definitions to recalcuate the correct Gaussian Curvature.
***************************************************************************/

class KPercentile
{
  public:
    float k_percentile_max = 0.90f;
    float k_percentile_min = 0.10f;

    /**
     * Function to apply k_percentile on a vector
    */
    vector<double> init(vector<float> &in_vector){
        // to find best values

        float index_max = in_vector.size() * k_percentile_max;
        float index_min = in_vector.size() * k_percentile_min;

        // max value
        sort(in_vector.begin(), in_vector.end());

        double max_value = 0;
        double min_value = 0;

        if(!(floor(index_max) == index_max)){ //not whole number
            index_max = round(index_max); // round it
            max_value = in_vector[index_max];
        } else {
            max_value = (in_vector[index_max] + in_vector[index_max + 1])/2;
        }


        if(!(floor(index_min) == index_min)){ //not whole number
            index_min = round(index_min); // round it
            min_value = in_vector[index_min];
        } else {
            min_value = (in_vector[index_min] + in_vector[index_min - 1])/2;
        }

        return vector<double>{max_value, min_value};
    }

};
#endif