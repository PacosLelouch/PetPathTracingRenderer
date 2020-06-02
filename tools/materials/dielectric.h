#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "material.h"
#include "../util.h"
#include "../objects/object.h"


double schlick(double cosTheta, double refIdx) {
    auto r0 = (1-refIdx) / (1+refIdx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosTheta),5);
}

class Dielectric: public Material{
public:
    Dielectric():albedo{0,0,0}, fuzzRate(0), refIdx(1.0){} 
    Dielectric(const RGB& albedo, double fuzzRate = 0, double refIdx = 1.0)
            :albedo(albedo), fuzzRate(fuzzRate), refIdx(refIdx){
        double etaRelative;
        if(refIdx < 1.0){
            fullReflectionAtBack = false;
            etaRelative = refIdx;
        }
        else{
            fullReflectionAtBack = true;
            etaRelative = 1.0 / refIdx;
        }
        minCosTheta = sqrt(1.0 - etaRelative * etaRelative);
    }
    
    virtual bool scatter(const Ray& ray, const HitRecord& hitRecord, 
            RGB& attenuation, Ray& scattered)const{
        attenuation = albedo / PI;
        double etaiOverEtat = hitRecord.front ? 1.0 / refIdx : refIdx;
        double cosTheta = dot(normalize(ray.direction()), -hitRecord.normal);
        double reflectProb = schlick(cosTheta, etaiOverEtat);
        if((fullReflectionAtBack ^ hitRecord.front) && cosTheta <= minCosTheta
                || randomDouble() < reflectProb){
            Vec3 scatterDir = Vec3::reflect(ray.direction(), hitRecord.normal);
            if(fuzzRate > 0){
                scatterDir += fuzzRate * Vec3::randomVectorSphere(1.0);
            }
            scattered = Ray(hitRecord.pos, scatterDir);
            return true;//dot(scattered.direction(), hitRecord.normal) >= 0;
        }
        Vec3 scatterDir = Vec3::refract(ray.direction(), hitRecord.normal, etaiOverEtat);
        scattered = Ray(hitRecord.pos, scatterDir);
        return true;//dot(scattered.direction(), hitRecord.normal) >= 0;
    }
    
    
protected:
    RGB albedo;
    double refIdx, fuzzRate, minCosTheta;
    bool fullReflectionAtBack;
};

#endif
