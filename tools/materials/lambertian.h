#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "material.h"
#include "../util.h"
#include "../objects/object.h"

class Lambertian: public Material{
public:
    Lambertian():albedo{0,0,0}{} 
    Lambertian(const RGB& albedo):albedo(albedo){}

    virtual bool scatter(const Ray& ray, const HitRecord& hitRecord, 
            RGB& attenuation, Ray& scattered)const{
        Vec3 scatterDir = Vec3::randomVectorHemisphere(1.0, hitRecord.normal);
            //hitRecord.normal + Vec3::randomVectorSphere(0.999);
            //hitRecord.normal + Vec3::randomVectorPillar(0.999);
        scattered = Ray(hitRecord.pos, scatterDir);
        attenuation = albedo / PI;
        return true;
    }
    
protected:
    RGB albedo;
};

#endif
