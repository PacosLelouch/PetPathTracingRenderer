#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"

class Camera{
public:
    Camera(){
        lowerLeftCorner = Point3(-2.0, -1.0, -1.0);
        horizontal = Vec3(4.0, 0.0, 0.0);
        vertical = Vec3(0.0, 2.0, 0.0);
        xAxis = normalize(horizontal);
        yAxis = normalize(vertical);
        zAxis = cross(xAxis, yAxis); 
        pos = Point3(0.0, 0.0, 0.0);
        lensRadius = 0;
        center = lowerLeftCorner + 0.5*horizontal + 0.5*vertical;
    }
    
    Camera(const Point3& pos, const Vec3& horizontal, const Vec3& vertical, 
            const Point3& lowerLeftCorner, double aperture = 0)
            :lowerLeftCorner(lowerLeftCorner),
            horizontal(horizontal),
            vertical(vertical),
            pos(pos),
            lensRadius(aperture*0.5),
            center(lowerLeftCorner + 0.5*horizontal + 0.5*vertical)
            {
        xAxis = normalize(horizontal);
        yAxis = normalize(vertical);
        zAxis = cross(xAxis, yAxis); 
    }
            
    Camera(const Point3& pos, const Point3& lookAt, const Vec3& up,
            double vfov, double aspectRatio, 
            double aperture = 0, double focusDist = 1.0)
            :pos(pos), lensRadius(aperture*0.5){
        auto theta = degrees2radians(vfov);
        auto halfHeight = tan(theta*0.5);
        auto halfWidth = aspectRatio * halfHeight;
        // right-hand coordinate
        zAxis = normalize(pos - lookAt);
        xAxis = normalize(cross(up, zAxis));
        yAxis = cross(zAxis, xAxis);
        
        lowerLeftCorner = pos - halfWidth*focusDist*xAxis - halfHeight*focusDist*yAxis - focusDist*zAxis;
        horizontal = 2*halfWidth*focusDist*xAxis;
        vertical = 2*halfHeight*focusDist*yAxis;
        center = pos - focusDist*zAxis;
    }
    
    Camera(const Camera& camera)
            :lowerLeftCorner(camera.lowerLeftCorner),
            horizontal(camera.horizontal),
            vertical(camera.vertical),
            pos(camera.pos),
            lensRadius(camera.lensRadius),
            center(camera.center){}
    
    Ray getRayUV(double u, double v)const{
        if(lensRadius > 0){
            Vec3 randomDisk = //Vec3::randomVectorCircle(randomDouble(0, lensRadius));
                Vec3::randomVectorInDisk(lensRadius);
            Vec3 offset = xAxis*randomDisk.x() + yAxis*randomDisk.y();
            //Vec3 offset = xAxis * (u*2-1) + yAxis* (v*2-1);
            return Ray(
                pos + offset, 
                lowerLeftCorner + u*horizontal + v*vertical - pos - offset);
        }
        return Ray(pos, lowerLeftCorner + u*horizontal + v*vertical - pos);
    }
    
    Ray getRayXY(double x, double y)const{
        return getRayUV(x*0.5 + 0.5, y*0.5 + 0.5);
    }
protected:
    Point3 pos, lowerLeftCorner, center;
    Vec3 horizontal, vertical, xAxis, yAxis, zAxis;
    double lensRadius;
};

#endif
