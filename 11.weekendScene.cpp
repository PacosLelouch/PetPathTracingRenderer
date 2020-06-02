#include <iostream>
#include <fstream>
#include <ctime>
#include "tools/ppmMSAA.h"
#include "tools/camera.h"
#include "tools/ray.h"
#include "tools/objects/objectList.h"
#include "tools/objects/sphere.h"
#include "tools/util.h"
#include "tools/materials/lambertian.h"
#include "tools/materials/metal.h"
#include "tools/materials/dielectric.h"

class PixelShader: public PixelCallback{
public:
    PixelShader():rejectRate(0), maxDepth(0), mode(0){
        cameraPtr = make_shared<Camera>();
        objectListPtr = make_shared<ObjectList>();
    }
    PixelShader(shared_ptr<Camera> cameraPtr, 
            shared_ptr<ObjectList> objectListPtr, 
            double rejectRate = 0.01, double maxDepth = 50)
            :cameraPtr(cameraPtr), objectListPtr(objectListPtr), 
            rejectRate(rejectRate), maxDepth(maxDepth), mode(0){}
    void switchMode(int mode){ this->mode = mode; }
    virtual ~PixelShader(){}
    virtual RGB operator()(double x, double y){
        Ray ray = cameraPtr->getRayXY(x, y);
        if(mode == 0){
            return shade(ray);
        }
        else{
            return shadeByDepth(ray, maxDepth);
        }
    }
    
protected:
    RGB backgroundColor(const Ray& r){
        Vec3 dir = normalize(r.direction());
        auto t = 0.5 * (dir.y() + 1.0);
        return interpolate(
            Color(1.0, 1.0, 1.0),
            Color(0.5, 0.7, 1.0),
            t);
    }
    
    RGB shade(const Ray& ray){
        HitRecord hitRecord;
        double r = randomDouble(0.0, 1.0);
        if(r < rejectRate){
            return RGB();
        }
        double compensation = 1 / (1 - rejectRate);
        if(objectListPtr->hit(ray, &hitRecord, TINY, INF)){
            Ray scattered;
            RGB attenuation;
            if(hitRecord.matPtr && 
                    hitRecord.matPtr->scatter(ray, hitRecord, attenuation, scattered)){
                return compensation * attenuation * shade(scattered);
            }
            return RGB();
        }
        return compensation * backgroundColor(ray);
    }
    
    RGB shadeByDepth(const Ray& ray, int depth){
        if(depth <= 0){
            return RGB();
                //shade(ray);
        }
        HitRecord hitRecord;
        if(objectListPtr->hit(ray, &hitRecord, TINY, INF)){
            Ray scattered;
            RGB attenuation;
            if(hitRecord.matPtr && 
                    hitRecord.matPtr->scatter(ray, hitRecord, attenuation, scattered)){
                return attenuation * shadeByDepth(scattered, depth - 1);
            }
            return RGB();
        }
        return backgroundColor(ray);
    }
    
    shared_ptr<Camera> cameraPtr;
    shared_ptr<ObjectList> objectListPtr;
    double rejectRate;
    int mode, maxDepth;
};

shared_ptr<ObjectList> randomScene(){
    auto worldPtr = make_shared<ObjectList>();
    worldPtr->add(make_shared<Sphere>(
        Point3(0,-1000,0), 1000, make_shared<Lambertian>(RGB(0.5, 0.5, 0.5)*PI)));
        
    int i = 1;
    auto albedoGlass = RGB(1.0, 1.0, 1.0)*PI;
    double fuzzGlass = 0.0;
    for(int a = -11; a < 11; ++a){
        for(int b = -11; b < 11; ++b){
            auto chooseMat = randomDouble();
            Point3 center(a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble());
            if((center - Vec3(4, 0.2, 0)).length() > 0.9){
                if(chooseMat < 0.8){
                    //diffuse
                    auto albedo = RGB::random() * RGB::random() * PI;
                    worldPtr->add(make_shared<Sphere>(
                        center, 0.2, 
                        make_shared<Lambertian>(albedo)));
                }
                else if(chooseMat < 0.95){
                    //metal
                    auto albedo = RGB::random(0.5, 1.0) * PI;
                    auto fuzz = randomDouble(0, 0.5);
                    worldPtr->add(make_shared<Sphere>(
                        center, 0.2,
                        make_shared<Metal>(albedo, fuzz)));
                }
                else{
                    //dielectric
                    worldPtr->add(make_shared<Sphere>(
                        center, 0.2,
                        make_shared<Dielectric>(albedoGlass, fuzzGlass, 1.5)));
                }
            }
        }
    }
    worldPtr->add(make_shared<Sphere>(
        Point3(0, 1, 0), 1.0,
        make_shared<Dielectric>(albedoGlass, fuzzGlass, 1.5)));
    worldPtr->add(make_shared<Sphere>(
        Point3(-4, 1, 0), 1.0,
        make_shared<Lambertian>(RGB(0.4, 0.2, 0.1)*PI)));
    worldPtr->add(make_shared<Sphere>(
        Point3(4, 1, 0), 1.0,
        make_shared<Metal>(RGB(0.7, 0.6, 0.5)*PI)));
    return worldPtr;
}

int main(){
    const auto aspectRatio = 16.0/9.0;
    const int imageWidth = 480;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    
    auto worldPtr = randomScene();
    
    Point3 pos(13,2,3);
    Point3 lookAt(0,0,0);
    Vec3 up(0,1,0);
    double vfov = 20.0;
    double distToFocus = 10.0;
    double aperture = 0.1;
    auto cameraPtr = 
        make_shared<Camera>(pos, lookAt, up, vfov, aspectRatio, aperture, distToFocus);
    PixelShader pixelShader(cameraPtr, worldPtr, 0.01, 48);
    pixelShader.switchMode(1);
    
    PPMMSAA ppm(imageWidth, imageHeight, 128, 0.5);
    double start = clock();
    ppm.shadePerPixel(&pixelShader);
    double end = clock();
    std::cout << "Shading used time " << double(end - start) / 1000 << "(s)." << std::endl;
    ppm.writeFile("pictures/weekendSceneGamma.ppm", false, GAMMA);
    ppm.writeFile("pictures/weekendSceneHDR2.ppm", false, HDR, 2.0);
    return 0;
} 
