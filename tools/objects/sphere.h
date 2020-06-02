#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"

class Sphere: public Object{
public:
    Sphere():radius(0){}
    Sphere(const Sphere& sphere):Object(sphere.pos), radius(sphere.radius){}
    Sphere(const Point3& pos, double radius, shared_ptr<Material> matPtr = nullptr):Object(pos, matPtr), radius(radius){}
    virtual ~Sphere(){}
    virtual bool hit(const Ray& ray, HitRecord* hitRecordPtr = nullptr, double tMin = 0.0, double tMax = std::numeric_limits<double>::max())const{
        Vec3 A = ray.position(), C = pos;
        Vec3 CtoA = A - C;
        double a = ray.direction().lengthSquared();
        double bHalf = dot(ray.direction(), CtoA);
        double c = CtoA.lengthSquared() - radius*radius;
        double discriminantQuarter = bHalf*bHalf - a*c;
        if(discriminantQuarter <= 0){
            return false;
        }
        double sqrtDHalf = sqrt(discriminantQuarter);
        double t1 = (-sqrtDHalf - bHalf) / a;
        bool outside = (pos - ray.position()).lengthSquared() > radius*radius;
        if(t1 >= tMin && t1 <= tMax){
            if(hitRecordPtr){
                hitRecordPtr->t = t1;
                hitRecordPtr->pos = ray.at(t1);
                hitRecordPtr->normal = normVec(hitRecordPtr->pos, outside);
                hitRecordPtr->front = outside;
                hitRecordPtr->matPtr = matPtr;
            }
            return true;
        }
        double t2 = (sqrtDHalf - bHalf) / a;
        if(t2 >= tMin && t2 <= tMax){
            if(hitRecordPtr){
                hitRecordPtr->t = t2;
                hitRecordPtr->pos = ray.at(t2);
                hitRecordPtr->normal = normVec(hitRecordPtr->pos, outside);
                hitRecordPtr->front = outside;
                hitRecordPtr->matPtr = matPtr;
            }
            return true;
        }
        return false;
    }
    virtual Vec3 normVec(const Point3& hitPoint, bool outside = true)const{
        return outside ? (hitPoint - pos) / radius : (pos - hitPoint) / radius;
    }
protected:
    double radius;
};

#endif
