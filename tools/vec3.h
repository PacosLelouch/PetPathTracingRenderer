#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream> 
#include "util.h"

using std::sqrt;

class Vec3;
inline std::ostream& operator<<(std::ostream &out, const Vec3 &v);
inline Vec3 operator+(const Vec3 &u, const Vec3 &v);
inline Vec3 operator+(const Vec3 &u, double t);
inline Vec3 operator+(double t, const Vec3 &u);
inline Vec3 operator-(const Vec3 &u, const Vec3 &v);
inline Vec3 operator-(const Vec3 &u, double t);
inline Vec3 operator-(double t, const Vec3 &u);
inline Vec3 operator*(const Vec3 &u, const Vec3 &v);
inline Vec3 operator*(double t, const Vec3 &v);
inline Vec3 operator*(const Vec3 &v, double t);
inline Vec3 operator/(const Vec3& v, double t);
inline double dot(const Vec3 &u, const Vec3 &v);
inline Vec3 cross(const Vec3 &u, const Vec3 &v);
inline Vec3 normalize(const Vec3& v);

class Vec3{
public:
    Vec3(const Vec3& v): e{v.e[0], v.e[1], v.e[2]}{}
    Vec3(double e0 = 0.0, double e1 = 0.0, double e2 = 0.0): e{e0, e1, e2}{}
    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }
    double r() const { return e[0]; }
    double g() const { return e[1]; }
    double b() const { return e[2]; }
    
    Vec3& operator=(const Vec3& v){
        this->e[0] = v.e[0];
        this->e[1] = v.e[1];
        this->e[2] = v.e[2];
        return *this;
    }
    Vec3 operator-() const{ return Vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const{ return e[i];}
    double& operator[](int i){return e[i];}
    
    Vec3& operator+=(const Vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vec3& operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3& operator/=(const double t) {
        return *this *= 1/t;
    }

    double length() const {
        return sqrt(lengthSquared());
    }

    double lengthSquared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }
    
    Vec3 normalized() const {
        double len = length();
        return Vec3(e[0] / len, e[1] / len, e[2] / len);
    }
    
    inline static Vec3 random(double min = 0.0, double max = 1.0){
        return Vec3(
            randomDouble(min, max), 
            randomDouble(min, max), 
            randomDouble(min, max));
    }
    
    inline static Vec3 randomVectorCircle(double radius = 1.0){
        auto phi = randomDouble(0, 2*PI);
        return Vec3(
            radius*cos(phi),
            radius*sin(phi),
            0);
    }
    
    inline static Vec3 randomVectorInDisk(double maxRadius = 1.0){
        auto x = randomDouble(-maxRadius, maxRadius);
        auto border = sqrt(maxRadius*maxRadius - x*x);
        auto y = randomDouble(-border, border);
        return Vec3(x, y, 0);
    }
    
    inline static Vec3 randomVectorSphere(double radius = 1.0){
        auto phi = randomDouble(0, 2*PI);
        auto theta = randomDouble(0, PI);
        return Vec3(
            radius*sin(theta)*cos(phi),
            radius*sin(theta)*sin(phi),
            radius*cos(theta));
    }
    
    inline static Vec3 randomVectorPillar(double radius = 1.0){
        auto z = randomDouble(-1, 1);
        auto phi = randomDouble(0, 2*PI);
        auto r = sqrt(1 - z*z);
        return Vec3(
            radius*r*cos(phi), 
            radius*r*sin(phi), 
            radius*z);
    }
    
    inline static Vec3 randomVectorHemisphere(double radius = 1.0, const Vec3& axis = Vec3(0, 0, 1)){
        Vec3 vec = randomVectorSphere(radius);
        if(dot(vec, axis) < 0){
            return -vec;
        }
        return vec;
    }
    
    // norm is a unit vector
    inline static Vec3 reflect(const Vec3& in, const Vec3& norm){
        Vec3 out = in - 2*dot(in, norm)*norm;
        return out;
    }
    
    inline static Vec3 refract(const Vec3& in, const Vec3& norm, double etaiOverEtat){
        auto length = in.length();
        Vec3 inUnit = in / length;
        auto cosTheta = dot(-inUnit, norm);
        Vec3 inUnitPerp = -cosTheta * norm;
        Vec3 inUnitPara = inUnit - inUnitPerp;
        Vec3 outUnitPara = etaiOverEtat * inUnitPara;
        Vec3 outUnitPerp = -sqrt(1 - outUnitPara.lengthSquared()) * norm;
        return length * (outUnitPara + outUnitPerp);
    }
    
    // norm is a unit vector
    inline static Vec3 halfVec(const Vec3& in, const Vec3& norm){
        return normalize(normalize(in) + norm);
    }
protected:
    double e[3];
};

// Vec3 Utility Functions

inline std::ostream& operator<<(std::ostream &out, const Vec3 &v) {
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
    return Vec3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

inline Vec3 operator+(const Vec3 &u, double t) {
    return Vec3(u[0] + t, u[1] + t, u[2] + t);
}

inline Vec3 operator+(double t, const Vec3 &u) {
    return u + t;
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
    return Vec3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

inline Vec3 operator-(const Vec3 &u, double t) {
    return Vec3(u[0] - t, u[1] - t, u[2] - t);
}

inline Vec3 operator-(double t, const Vec3 &u) {
    return Vec3(t - u[0], t - u[1], t - u[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
    return Vec3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

inline Vec3 operator*(double t, const Vec3 &v) {
    return Vec3(t*v[0], t*v[1], t*v[2]);
}

inline Vec3 operator*(const Vec3 &v, double t) {
    return t * v;
}

inline Vec3 operator/(const Vec3& v, double t) {
    return (1/t) * v;
}

inline double dot(const Vec3 &u, const Vec3 &v) {
    return u[0] * v[0]
         + u[1] * v[1]
         + u[2] * v[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
    return Vec3(u[1] * v[2] - u[2] * v[1],
                u[2] * v[0] - u[0] * v[2],
                u[0] * v[1] - u[1] * v[0]);
}

inline Vec3 normalize(const Vec3& v) {
    return v / v.length();
}

using Point3 = Vec3;
using Color = Vec3;
using RGB = Vec3;

#endif
