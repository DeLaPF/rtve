#pragma once

#include <memory>

#include "glm/glm.hpp"

#include "Walnut/Image.h"

class Renderer
{
public:
    Renderer() = default;

    void Render();

    void OnResize(uint32_t width, uint32_t height);

    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; };
private:
    uint32_t PerPixel(glm::vec2 coord);

    uint32_t DrawSphere(glm::vec2 coord);

    bool DoesRayHitSphere(glm::vec4* rayOrigin, glm::vec3* direction, glm::vec4* sphereOrigin, float radius);
private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData;

    // x, y, z, 'w' is focalLength
    glm::vec4 m_Camera = glm::vec4(0.0f, 0.0f, -4.0f, 1.0f); 
    // x, y, z, 'w' is raidus
    glm::vec4 m_Sphere = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

};
