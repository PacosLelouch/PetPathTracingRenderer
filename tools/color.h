#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "util.h"

#include <iostream>

using std::pow;
using std::exp;

class PixelCallback{
public:
    // x, y in [-1, 1]
    virtual RGB operator()(double x, double y) = 0;
};

bool writeRGB(std::ostream &out, const RGB& pixelRGB, bool verbose = true) {
    // Write the translated [0,255] value of each color component.
    int r = static_cast<int>(255.999 * pixelRGB.r());
    int g = static_cast<int>(255.999 * pixelRGB.g());
    int b = static_cast<int>(255.999 * pixelRGB.b());
    bool result = true;
    if(r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255){
        if(verbose){
            std::cerr << "pixel [" << r << ',' << g << ',' << b
                << "] is out of range." << std::endl;
        }
        result = false;
    }
    out << clamp(r, 0, 255) << ' '
        << clamp(g, 0, 255) << ' '
        << clamp(b, 0, 255) << '\n';
    return result;
}

bool writeGammaRGB(std::ostream &out, const RGB& pixelRGB, bool verbose = true, double gamma = 2.2){
    // Write the translated [0,255] value of each color component.
    double invGamma = 1 / 2.2;
    int r = static_cast<int>(255.999 * pow(pixelRGB.r(), invGamma));
    int g = static_cast<int>(255.999 * pow(pixelRGB.g(), invGamma));
    int b = static_cast<int>(255.999 * pow(pixelRGB.b(), invGamma));
    bool result = true;
    if(r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255){
        if(verbose){
            std::cerr << "pixel [" << r << ',' << g << ',' << b
                << "] is out of range." << std::endl;
        }
        result = false;
    }
    out << clamp(r, 0, 255) << ' '
        << clamp(g, 0, 255) << ' '
        << clamp(b, 0, 255) << '\n';
    return result;
}

bool writeHDR2RGB(std::ostream &out, const RGB& pixelHDR, bool verbose = true, double exposure = 1.0){
    // Write the translated [0,255] value of each color component.
    double gamma = 2.2, invGamma = 1 / 2.2;
    RGB pixelRGB = RGB(
        1.0 - exp(-pixelHDR.r() * exposure),
        1.0 - exp(-pixelHDR.g() * exposure),
        1.0 - exp(-pixelHDR.b() * exposure));
    int r = static_cast<int>(255.999 * pow(pixelRGB.r(), invGamma));
    int g = static_cast<int>(255.999 * pow(pixelRGB.g(), invGamma));
    int b = static_cast<int>(255.999 * pow(pixelRGB.b(), invGamma));
    bool result = true;
    if(r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255){
        if(verbose){
            std::cerr << "pixel [" << r << ',' << g << ',' << b
                << "] is out of range." << std::endl;
        }
        result = false;
    }
    out << clamp(r, 0, 255) << ' '
        << clamp(g, 0, 255) << ' '
        << clamp(b, 0, 255) << '\n';
    return result;
}

#endif
