#ifndef OBJECT_LIST_H
#define OBJECT_LIST_H

#include "../ray.h"
#include "object.h"
#include <limits>
#include <vector>
#include <memory>

using std::shared_ptr;
using std::make_shared;

class ObjectList{
public:
    ObjectList(){}
    ObjectList(shared_ptr<Object> object){ add(object); }
    
    void clear(){ objects.clear(); }
    void add(shared_ptr<Object> object){ objects.push_back(object); }
    virtual bool hit(const Ray& ray, HitRecord* hitRecordPtr = nullptr, double tMin = 0.0, double tMax = std::numeric_limits<double>::max()){
        bool hitAnything = false;
        HitRecord tempHitRecord(tMax);
        auto currentClosest = tMax;
        for(const auto& object: objects){
            if(object->hit(ray, &tempHitRecord, tMin, currentClosest)){
                hitAnything = true;
                if(currentClosest > tempHitRecord.t){
                    currentClosest = tempHitRecord.t;
                    if(hitRecordPtr){
                        hitRecordPtr->copy(tempHitRecord);
                    }
                }
            }
        }
        return hitAnything;
    }
    virtual Vec3 normVec(const Point3& hitPoint, bool outside = true){
        return Vec3(0,0,0);
    }
protected:
    std::vector<shared_ptr<Object> > objects;
};

#endif
