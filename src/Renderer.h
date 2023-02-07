#pragma once

#include "Walnut/Image.h"

#include "Camera/Camera.h"
#include "Ray.h"
#include "Scene.h"

#include <cstdint>
#include <memory>
#include <cstring>
#include "glm/glm.hpp"

class Renderer
{
public:
    struct Settings
    {
        bool Accumulate = true;
        bool Multithread = true;
        int Bounces = 2;
    };

    Renderer() = default;

    void Render(const Scene& scene, const Camera& camera);

    void OnResize(uint32_t width, uint32_t height);

    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; };

    void ResetFrameIndex() { m_FrameIndex = 1; };

    Settings& GetSettings() { return m_Settings; }
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
    glm::vec4* m_AccumulationData = nullptr;
    uint32_t m_FrameIndex = 1;
    float m_AspectRatio = 1.0f;

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;

    Settings m_Settings;
    std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;
};
