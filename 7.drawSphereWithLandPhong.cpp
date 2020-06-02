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

class PixelShader: public PixelCallback{
public:
    PixelShader(){}
    PixelShader(const Camera& camera, const ObjectList& objectList, double rejectRate = 0.01, double maxDepth = 50)
        :camera(camera), objectList(objectList), rejectRate(rejectRate),
        maxDepth(maxDepth), mode(0){}
    void switchMode(int mode){ this->mode = mode; }
    virtual ~PixelShader(){}
    virtual RGB operator()(double x, double y){
        Ray ray = camera.getRayXY(x, y);
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
        if(objectList.hit(ray, &hitRecord, TINY, INF)){
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
        if(objectList.hit(ray, &hitRecord, TINY, INF)){
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
    
    Camera camera;
    ObjectList objectList;
    double rejectRate;
    int mode, maxDepth;
};

int main(){
    const auto aspect_ratio = 16.0/9.0;
    const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    
    ObjectList world;
    // Land
    world.add(make_shared<Sphere>(
        Point3(0,0,-1), 0.5,
        make_shared<Lambertian>(RGB(0.7, 0.3, 0.3) * PI)));
    // Lambertian Sphere
    world.add(make_shared<Sphere>(
        Point3(0,-100.5,-1), 100,
        make_shared<Lambertian>(RGB(0.8, 0.8, 0.0) * PI)));
    // Metal Sphere
    world.add(make_shared<Sphere>(
        Point3(1,0,-1), 0.5,
        make_shared<Metal>(RGB(0.8, 0.6, 0.2) * PI, 0.3)));
    // Metal Sphere
    world.add(make_shared<Sphere>(
        Point3(-1,0,-1), 0.5,
        make_shared<Metal>(RGB(0.8, 0.8, 0.8) * PI, 0.8)));
    
    Vec3 horizontal(4.0, 0.0, 0.0);
    Vec3 vertical(0.0, 2.25, 0.0);
    Point3 pos(0.0, 0.0, 0.0);
    Point3 lowerLeftCorner(-0.5*horizontal.x(), -0.5*vertical.y(), -1.0);
    
    Camera camera(pos, horizontal, vertical, lowerLeftCorner);
    
    PixelShader pixelShader(camera, world, 0.01, 50);
    pixelShader.switchMode(1);
    
    PPMMSAA ppm(image_width, image_height, 128, 0.5);
    double start = clock();
    ppm.shadePerPixel(&pixelShader);
    double end = clock();
    std::cout << "Shading used time " << double(end - start) / 1000 << "(s)." << std::endl;
    ppm.writeFile("pictures/sphereWithLandPhongGamma.ppm", false, GAMMA);
    ppm.writeFile("pictures/sphereWithLandPhongHDR2.ppm", false, HDR, 2.0);
    return 0;
} 
