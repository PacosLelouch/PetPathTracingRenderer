#ifndef OBJECT_H
#define OBJECT_H

#include "../ray.h"
#include "../materials/material.h"
#include <limits>
#include <memory>

using std::shared_ptr;

struct HitRecord{
    Point3 pos;
    Vec3 normal;
    bool front;
    double t;
    shared_ptr<Material> matPtr;
    
    HitRecord(double t = 0, shared_ptr<Material> matPtr = nullptr):t(t), matPtr(matPtr){}
    void copy(const HitRecord& hitRecord){
        t = hitRecord.t;
        normal = hitRecord.normal;
        pos = hitRecord.pos;
        front = hitRecord.front;
        matPtr = hitRecord.matPtr;
    }
};

class Object{
public:
    Object():pos{0,0,0}{}
    Object(const Object& object):pos(object.pos),matPtr(object.matPtr){}
    Object(const Point3& pos, shared_ptr<Material> matPtr = nullptr):pos(pos), matPtr(matPtr){}
    virtual Vec3 position()const{ return pos; }
    virtual ~Object(){}
    virtual bool hit(const Ray& ray, HitRecord* hitRecordPtr = nullptr, double tMin = 0.0, double tMax = std::numeric_limits<double>::max())const{
        Vec3 relativePos = pos - ray.position();
        double rayLength = ray.direction().length();
        double distance = relativePos.length();
        Vec3 rayDirNorm = ray.direction() / rayLength;
        Vec3 relativePosNorm = relativePos / distance;
        double cosTheta = dot(rayDirNorm, relativePosNorm);
        if(cosTheta < 1.0){
            return false;
        }
        double t = cosTheta * distance / rayLength;
        if(t < tMin || t > tMax){
            return false;
        }
        if(hitRecordPtr){
            hitRecordPtr->t = t;
            hitRecordPtr->front = true;
            hitRecordPtr->normal = Vec3(0,0,0); 
            hitRecordPtr->pos = ray.at(t);
            hitRecordPtr->matPtr = matPtr;
        }
        return true;
    }
    virtual Vec3 normVec(const Point3& hitPoint, bool outside = true)const{
        return Vec3(0,0,0);
    }
protected:
    Point3 pos;
    shared_ptr<Material> matPtr;
};

#endif
