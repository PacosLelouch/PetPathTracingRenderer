#include <iostream>
#include <fstream>
#include "tools/ppm.h"
#include "tools/ray.h"
#include "tools/util.h"

Color rayColor(const Ray& r){
    Vec3 dir = normalize(r.direction());
    auto t = 0.5 * (dir.y() + 1.0);
    return interpolate(
        Color(1.0, 1.0, 1.0),
        Color(0.5, 0.7, 1.0),
        t);
}

class RayCallback: public PixelCallback{
public:
    RayCallback(){
        origin = Point3(0.0, 0.0, 0.0);
        horizontal = Vec3(4.0, 0.0, 0.0);
        vertical = Vec3(0.0, 2.25, 0.0);
        lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, 1);
    }
    ~RayCallback(){}
    virtual RGB operator()(double x, double y){
        auto u = x / 2 + 0.5;
        auto v = y / 2 + 0.5;
        Ray ray(origin, lowerLeftCorner + u * horizontal + v * vertical);
        return rayColor(ray);
    }
private:
    Point3 origin, lowerLeftCorner;
    Vec3 horizontal, vertical;
};

int main(){
    const auto aspect_ratio = 16.0/9.0;
    const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    
    PPM ppm(image_width, image_height);
    RayCallback rayCallback;
    ppm.shadePerPixel(&rayCallback);
    ppm.writeFile("pictures/ray.ppm", true);
    return 0;
} 
