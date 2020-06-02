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

int main(){
    const auto aspectRatio = 16.0/9.0;
    const int imageWidth = 384;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    
    auto worldPtr = make_shared<ObjectList>();
    // Land
    worldPtr->add(make_shared<Sphere>(
        Point3(0,0,-1), 0.5,
        make_shared<Lambertian>(RGB(0.1, 0.2, 0.5) * PI)));
    // Lambertian Sphere
    worldPtr->add(make_shared<Sphere>(
        Point3(0,-100.5,-1), 100,
        make_shared<Lambertian>(RGB(0.8, 0.8, 0.0) * PI)));
    // Metal Sphere
    worldPtr->add(make_shared<Sphere>(
        Point3(1,0,-1), 0.5,
        make_shared<Metal>(RGB(0.8, 0.6, 0.2) * PI, 0.1)));
    // Dielectric Sphere
    worldPtr->add(make_shared<Sphere>(
        Point3(-1,0,-1), 0.5,
        make_shared<Dielectric>(RGB(0.95, 0.95, 0.95) * PI, 0.0, 1.5)));
    // Dielectric Sphere
    worldPtr->add(make_shared<Sphere>(
        Point3(0,1.25,-1.25), 0.5,
        make_shared<Dielectric>(RGB(0.95, 0.95, 0.95) * PI, 0.0, 1.2)));
    // Dielectric Sphere inside (inverse)
    worldPtr->add(make_shared<Sphere>(
        Point3(0,1.25,-1.25), -0.4,
        make_shared<Dielectric>(RGB(0.95, 0.95, 0.95) * PI, 0.0, 1.2)));
    
    Point3 pos(3,3,2);
    Point3 lookAt(0,0,-1);
    Vec3 up(0,1,0);
    double vfov = 20.0;
    double distToFocus = (pos - lookAt).length();
    double aperture = 2.0;
    auto cameraPtr = 
        make_shared<Camera>(pos, lookAt, up, vfov, aspectRatio, aperture, distToFocus);
    PixelShader pixelShader(cameraPtr, worldPtr, 0.01, 32);
    pixelShader.switchMode(1);
    
    PPMMSAA ppm(imageWidth, imageHeight, 128, 0.5);
    double start = clock();
    ppm.shadePerPixel(&pixelShader);
    double end = clock();
    std::cout << "Shading used time " << double(end - start) / 1000 << "(s)." << std::endl;
    ppm.writeFile("pictures/defocusBlur.ppm", false, GAMMA);
    return 0;
} 
