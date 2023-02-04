#pragma once

#include "Walnut/Image.h"

#include "Camera/Camera.h"
#include "Ray.h"
#include "Scene.h"

#include <cstdint>
#include <memory>
#include "glm/glm.hpp"

class Renderer
{
public:
    Renderer() = default;

    void Render(const Scene& scene, const Camera& camera);

    void OnResize(uint32_t width, uint32_t height);

    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; };
private:
    struct HitPayload
    {
        float HitDistance;
        glm::vec3 WorldPosition;
        glm::vec3 WorldNormal;

        uint32_t ObjectIndex;
    };

    glm::vec4 RayGen(uint32_t x, uint32_t y);

    HitPayload TraceRay(const Ray& ray);
    HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
    HitPayload Miss(const Ray& ray);
private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData = nullptr;

    float m_AspectRatio = 1.0f;

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;
};
