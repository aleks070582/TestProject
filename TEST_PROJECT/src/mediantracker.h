#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/median.hpp>

namespace ba = boost::accumulators;

    class MedianTracker {
    private:
        ba::accumulator_set<double, ba::stats<ba::tag::median(ba::with_p_square_quantile)>> acc;
        double current_median = 0.0;
        bool first_value = true;
        bool second_value = true;
        const double EPSILON = 1e-9;
    public:
        bool addValue(double value) {
        acc(value);
        //чтобы "создать" медиану для 1го и 2го значения которе передается в аккумулятор
        if (first_value) {
            current_median = value;
            first_value = false;
            return true;  
        }
        if(second_value){
            double old_median = current_median;
            current_median = (current_median + value)/2;
            bool changed = std::abs(current_median - old_median) > EPSILON;
            second_value = false;
            return changed;
        }
        double old_median = current_median;
        double new_median = ba::median(acc);
        bool changed = std::abs(new_median - old_median) > EPSILON;
            if (changed) {
                current_median = new_median;
            }
            return changed;
        }
        
        double getMedian() const {
            return current_median;
        }
    };
