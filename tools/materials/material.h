#ifndef MATERIAL_H
#define MATERIAL_H

#include "../ray.h"

class HitRecord;

class Material{
public:
    virtual bool scatter(const Ray& ray, const HitRecord& hitRecord, RGB& attenuation, Ray& scattered)const = 0;
};

#endif
