#pragma once

#include "Walnut/Image.h"

#include "Camera/Camera.h"
#include "Ray.h"

#include <memory>
#include "glm/glm.hpp"

class Renderer
{
public:
    Renderer() = default;

    void Render(const Camera& camera);

    void OnResize(uint32_t width, uint32_t height);

    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; };

    void SetSphereColor(glm::vec4 floatColor) { m_SphereColor = floatColor; }

    void SetSphereLocation(glm::vec3 location) { m_Sphere = location; }

    void SetLightDirection(glm::vec3 direction) { m_LightDirection = direction; }
private:
    glm::vec4 TraceRay(const Ray& ray);

    glm::vec4 DrawSphere(const Ray& ray);
private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData;
    float m_AspectRatio = 1.0f;

    glm::vec3 m_Camera = glm::vec3(0.0f, 0.0f, 1.0f); 
    float m_CameraFocalLength = -1.0f;

    glm::vec3 m_Sphere = glm::vec3(0.0f, 0.0f, 0.0f);
    float m_SphereRadius = 0.5f;
    glm::vec4 m_SphereColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    glm::vec3 m_LightDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
};
