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
    float k_percentile = 0.90f;

    /**
     * Function to apply k_percentile on a vector
    */
    double init(vector<float> &in_vector, vector<float> &out_vector){
        sort(in_vector.begin(), in_vector.end());
        float index = in_vector.size() * k_percentile;

        double max_value;

        if(!(floor(index) == index)){ //not whole number
            index = round(index); // round it
            max_value = in_vector[index];
        } else {
            max_value = (in_vector[index] + in_vector[index + 1])/2;
        }

        for(int k = 0; k < index; k++){
            out_vector.push_back(in_vector[k]);
        }

        return max_value;
    }

    /**
     * Function to set the k percentile
    */
    void set_k_percentile(float k){
        k_percentile = k;
    }
};
#endif