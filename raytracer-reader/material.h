#pragma once

#include <vector.h>
#include <string>

struct Material {
    std::string name;
    Vector ambient_color;
    Vector diffuse_color;
    Vector specular_color;
    Vector intensity;
    double specular_exponent;
    double refraction_index;
    Vector albedo;

    Material() : intensity(0, 0, 0), refraction_index(1), albedo{1, 0, 0} {
    }
};
