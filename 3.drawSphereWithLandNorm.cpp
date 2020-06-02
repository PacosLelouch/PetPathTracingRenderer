#include <iostream>
#include <fstream>
#include "tools/ppm.h"
#include "tools/ray.h"
#include "tools/objects/objectList.h"
#include "tools/objects/sphere.h"
#include "tools/util.h"

Color backColor(const Ray& r){
    Vec3 dir = normalize(r.direction());
    auto t = 0.5 * (dir.y() + 1.0);
    return interpolate(
        Color(1.0, 1.0, 1.0),
        Color(0.5, 0.7, 1.0),
        t);
}

class SphereWithLandCallback: public PixelCallback{
public:
    SphereWithLandCallback(const ObjectList& objectList):objectList(objectList){
        origin = Point3(0.0, 0.0, 0.0);
        horizontal = Vec3(4.0, 0.0, 0.0);
        vertical = Vec3(0.0, 2.25, 0.0);
        lowerLeftCorner = origin - horizontal * 0.5 - vertical * 0.5 - Vec3(0, 0, 1);
    }
    ~SphereWithLandCallback(){}
    virtual RGB operator()(double x, double y){
        auto u = x * 0.5 + 0.5;
        auto v = y * 0.5 + 0.5;
        Ray ray(origin, lowerLeftCorner + u * horizontal + v * vertical);
        HitRecord hitRecord;
        if(objectList.hit(ray, &hitRecord)){
            return hitRecord.normal * 0.5 + 0.5;
        }
        return backColor(ray);
    }
private:
    Point3 origin, lowerLeftCorner;
    Vec3 horizontal, vertical;
    ObjectList objectList;
};

int main(){
    const auto aspect_ratio = 16.0/9.0;
    const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    
    ObjectList world;
    world.add(make_shared<Sphere>(Point3(0,0,-1), 0.5));
    world.add(make_shared<Sphere>(Point3(0,-100.5,-1), 100));
    SphereWithLandCallback sphereWithLandCallback(world);
    
    PPM ppm(image_width, image_height);
    ppm.shadePerPixel(&sphereWithLandCallback);
    ppm.writeFile("pictures/sphereWithLandNorm.ppm", true);
    return 0;
} 
