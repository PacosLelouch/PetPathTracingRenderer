#include <iostream>
#include <fstream>
#include "tools/ppm.h"
#include "tools/ray.h"
#include "tools/objects/sphere.h"
#include "tools/util.h"

Color skyColor(const Ray& r){
    Vec3 dir = normalize(r.direction());
    auto t = 0.5 * (dir.y() + 1.0);
    return interpolate(
        Color(1.0, 1.0, 1.0),
        Color(0.5, 0.7, 1.0),
        t);
}

class SphereCallback: public PixelCallback{
public:
    SphereCallback(const Sphere& sphere):sphere(sphere){
        origin = Point3(0.0, 0.0, 0.0);
        horizontal = Vec3(4.0, 0.0, 0.0);
        vertical = Vec3(0.0, 2.25, 0.0);
        lowerLeftCorner = origin - horizontal * 0.5 - vertical * 0.5 - Vec3(0, 0, 1);
    }
    ~SphereCallback(){}
    virtual RGB operator()(double x, double y){
        auto u = x * 0.5 + 0.5;
        auto v = y * 0.5 + 0.5;
        Ray ray(origin, lowerLeftCorner + u * horizontal + v * vertical);
        HitRecord hitRecord;
        if(sphere.hit(ray, &hitRecord)){
            return hitRecord.normal * 0.5 + 0.5;
        }
        return skyColor(ray);
    }
private:
    Point3 origin, lowerLeftCorner;
    Vec3 horizontal, vertical;
    Sphere sphere;
};

int main(){
    const auto aspect_ratio = 16.0/9.0;
    const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    
    PPM ppm(image_width, image_height);
    SphereCallback sphereCallback(Sphere(Point3(0,0,-1), 0.5));
    ppm.shadePerPixel(&sphereCallback);
    ppm.writeFile("pictures/sphereNorm.ppm", true);
    return 0;
} 
