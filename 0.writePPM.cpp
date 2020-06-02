#include <iostream>
#include <fstream>
#include "tools/ppm.h"
#include "tools/util.h"

class RGBCallback: public PixelCallback{
public:
    RGBCallback(){}
    ~RGBCallback(){}
    virtual RGB operator()(double x, double y){
        return RGB(
            x / 2 + 0.5,
            y / 2 + 0.5,
            0.25);
    }
};

int main(){
    const int image_width = 256;
    const int image_height = 256;
    
    RGBCallback callback;
    
    PPM ppm(image_width, image_height);
    ppm.shadePerPixel(&callback);
    ppm.writeFile("pictures/rgb.ppm", true);
    return 0;
} 
