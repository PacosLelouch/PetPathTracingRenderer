#ifndef UTIL_H
#define UTIL_H

#include <limits>
#include <cmath>
#include <functional>
#include <random>
#include "vec3.h"

const double INF = std::numeric_limits<double>::max();
const double TINY = 1e-6;
const double PI = std::acos(-1.0);

template<typename T>
inline T clamp(T x, T min, T max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

template<typename T>
T interpolate(const T& begin, const T& end, double t){
    return (1.0 - t) * begin + t * end;
}

inline double randomDouble(double min = 0.0, double max = 1.0) {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    static std::function<double()> randGenerator =
        std::bind(distribution, generator);
    return interpolate(min, max, randGenerator());
}

inline double degrees2radians(double degrees) {
    return degrees * PI / 180;
}

#endif
