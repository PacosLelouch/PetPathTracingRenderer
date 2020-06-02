#ifndef PPM_H
#define PPM_H

#include <string>
#include <fstream>
#include "color.h"

class PPM{
public:
    PPM(int width = 256, int height = 256)
            :width(width), height(height){
        pixels = new RGB*[height];
        for(int h = 0; h < height; ++h){
            pixels[h] = new RGB[width];
        }
    }
    
    virtual ~PPM(){
        for(int h = 0; h < height; ++h){
            delete[] pixels[h];
        }
        delete[] pixels;
    }
    
    virtual void shadePerPixel(PixelCallback* callbackPtr, bool verbose = true){
        int count = 0, total = height * width, verboseStep = 16;
        for(int h = height - 1; h >= 0; --h){
            for(int w = 0; w < width; ++w){
                RGB pixel = (*callbackPtr)(
                    (double(w) / (width - 1)) * 2 - 1, 
                    (double(h) / (height - 1)) * 2 - 1);
                pixels[h][w] = pixel;
                ++count;
                if(verbose && count % verboseStep == 1){
                    std::cerr << "\rShading complete: " << count << '/' << total << std::flush;
                }
            }
        }
        if(verbose){
            std::cerr << "\rShading complete: " << total << '/' << total << std::endl;
        }
    }
    
    virtual void writeFile(const std::string& fname, bool verbose = true){
        out.open(fname);
        out << "P3\n" << width << ' ' << height << "\n255\n" << std::endl;
        for(int h = height - 1; h >= 0; --h){
            if(verbose)
                std::cerr << "\rScanlines remaining: " << h << ' ' << std::flush;
            for(int w = 0; w < width; ++w){
                if(!writeRGB(out, pixels[h][w], verbose) && verbose){
                    std::cerr << "Error at [" << h << ',' << w << ']' << std::endl;
                }
            }
        }
        if(verbose)
            std::cerr << "\nDone." << std::endl;
        out.close();
    }
protected:
    int width, height;
    RGB** pixels;
    std::ofstream out;
};

#endif
