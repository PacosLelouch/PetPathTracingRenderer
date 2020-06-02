#ifndef PPM_MSAA_H
#define PPM_MSAA_H

#include "ppm.h"
#include "util.h"

enum WriteWay{
    DIRECT,
    GAMMA,
    HDR
};

class PPMMSAA: public PPM{
public:
    PPMMSAA(int width = 256, int height = 256, int nSample = 4, double halfRange = 1.0)
            :PPM(width, height), nSample(nSample){
        randx = new double[nSample];
        randy = new double[nSample];
        for(int i = 0; i < nSample; ++i){
            randx[i] = randomDouble(-halfRange, halfRange);
            randy[i] = randomDouble(-halfRange, halfRange);
        }
    }
    
    virtual ~PPMMSAA(){
        delete[] randx;
        delete[] randy;
    }
    
    virtual void shadePerPixel(PixelCallback* callbackPtr, bool verbose = true){
        int count = 0, total = height * width * nSample, verboseStep = 16 * nSample;
        double scale = 1.0 / nSample;
        for(int h = height - 1; h >= 0; --h){
            for(int w = 0; w < width; ++w){
                pixels[h][w] = RGB();
                for(int i = 0; i < nSample; ++i){
                    double x = (w + randx[i]) / (width - 1) * 2 - 1, 
                        y = (h + randy[i]) / (height - 1) * 2 - 1;
                    RGB pixel = (*callbackPtr)(x, y);
                    pixels[h][w] += pixel;
                    ++count;
                    if(verbose && count % verboseStep == 1){
                        std::cerr << "\rShading complete: " << count << '/' << total << std::flush;
                    }
                }
                pixels[h][w] *= scale;
            }
        }
        if(verbose){
            std::cerr << "\rShading complete: " << total << '/' << total << std::endl;
        }
    }
    
    virtual void writeFile(const std::string& fname, bool verbose = true, 
        WriteWay writeWay = DIRECT, double exposureHDR = 1.0){
        out.open(fname);
        out << "P3\n" << width << ' ' << height << "\n255\n" << std::endl;
        for(int h = height - 1; h >= 0; --h){
            if(verbose)
                std::cerr << "\rScanlines remaining: " << h << ' ' << std::flush;
            for(int w = 0; w < width; ++w){
                bool okay = true;
                switch(writeWay){
                case DIRECT:
                    okay = writeRGB(out, pixels[h][w], verbose);
                    break;
                case GAMMA:
                    okay = writeGammaRGB(out, pixels[h][w], verbose, 2.2);
                    break;
                case HDR:
                default:
                    okay = writeHDR2RGB(out, pixels[h][w], verbose, exposureHDR);
                    break;
                }
                if(!okay && verbose){
                    std::cerr << "Error at [" << h << ',' << w << ']' << std::endl;
                }
            }
        }
        if(verbose)
            std::cerr << "\nDone." << std::endl;
        out.close();
    }
protected:
    int nSample;
    double* randx;
    double* randy;
};

#endif
