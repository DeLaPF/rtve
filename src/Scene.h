#pragma once

#include "glm/glm.hpp"

#include <vector>

struct Sphere
{
    glm::vec3 Position{0.0f};
    float Radius = 0.5f;

    glm::vec3 Albedo{1.0f};
    float Opacity = 1.0f;
};

struct Scene
{
    std::vector<Sphere> Spheres;

    glm::vec3 LightDirection{-1.0f};
    glm::vec4 BackgroundColor{0.0f, 0.0f, 0.0f, 1.0f};
};
