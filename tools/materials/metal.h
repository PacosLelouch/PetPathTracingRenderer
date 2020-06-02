#ifndef METAL_H
#define METAL_H

#include "material.h"
#include "../util.h"
#include "../objects/object.h"

class Metal: public Material{
public:
    Metal():albedo{0,0,0}, fuzzRate(0){} 
    Metal(const RGB& albedo, double fuzzRate = 0)
            :albedo(albedo), fuzzRate(fuzzRate < 1.0 ? fuzzRate : 1.0){}

    virtual bool scatter(const Ray& ray, const HitRecord& hitRecord, 
            RGB& attenuation, Ray& scattered)const{
        Vec3 scatterDir = Vec3::reflect(ray.direction(), hitRecord.normal);
        if(fuzzRate > 0){
            scatterDir += fuzzRate * Vec3::randomVectorSphere(1.0);
        }
        scattered = Ray(hitRecord.pos, scatterDir);
        attenuation = albedo / PI;
        return dot(scattered.direction(), hitRecord.normal) >= 0;
    }
    
protected:
    RGB albedo;
    double fuzzRate;
};

#endif
