#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray{
public:
    Ray(){}
    Ray(const Point3& pos, const Vec3& dir)
        :pos(pos), dir(dir){}
    
    Point3 position()const{ return pos;}
    Vec3 direction()const{ return dir;}
    
    Point3 at(double t)const{
        return pos + t * dir;
    } 
    
protected:
    Point3 pos;
    Vec3 dir;
};

#endif
