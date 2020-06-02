#include <iostream>
#include <fstream>
#include "tools/ppmMSAA.h"
#include "tools/camera.h"
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
    SphereWithLandCallback(){}
    SphereWithLandCallback(const Camera& camera, const ObjectList& objectList)
        :camera(camera), objectList(objectList){}
    virtual ~SphereWithLandCallback(){}
    virtual RGB operator()(double x, double y){
        Ray ray = camera.getRayXY(x, y);
        HitRecord hitRecord;
        if(objectList.hit(ray, &hitRecord)){
            return hitRecord.normal * 0.5 + 0.5;
        }
        return backColor(ray);
    }
protected:
    Camera camera;
    ObjectList objectList;
};

int main(){
    const auto aspect_ratio = 16.0/9.0;
    const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    
    ObjectList world;
    world.add(make_shared<Sphere>(Point3(0,0,-1), 0.5));
    world.add(make_shared<Sphere>(Point3(0,-100.5,-1), 100));
    
    Vec3 horizontal(4.0, 0.0, 0.0);
    Vec3 vertical(0.0, 2.25, 0.0);
    Point3 pos(0.0, 0.0, 0.0);
    Point3 lowerLeftCorner(-2.0, -1.125, -1.0);
    
    Camera camera(pos, horizontal, vertical, lowerLeftCorner);
    
    SphereWithLandCallback sphereWithLandCallback(camera, world);
    
    PPMMSAA ppm(image_width, image_height, 128, 0.5);
    ppm.shadePerPixel(&sphereWithLandCallback);
    ppm.writeFile("pictures/sphereWithLandNormMSAA.ppm", true);
    return 0;
} 
