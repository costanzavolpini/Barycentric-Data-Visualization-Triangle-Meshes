#ifndef GCHELPER_H
#define GCHELPER_H

#include "Point3.h"
#include "Base.h"
#include <math.h>

using namespace std;

/***************************************************************************
GaussianCurvatureHelper.h
Comment:  This file contains all Statistics definitions to recalcuate the correct Gaussian Curvature.
***************************************************************************/

class GCHelper {
  public:
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

           cout << mean << endl;
           cout << lower_outlier << endl;
           cout << upper_outlier << endl;


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

            // interval from -1 to 1 then it is 2 because (-1+1)
            // I have mapped all values from -1 to 1
            return 2/(upper_outlier - lower_outlier) * (val - upper_outlier) + 1; //1 is the max of interval
       }

};
#endif