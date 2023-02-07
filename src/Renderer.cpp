#include "Renderer.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"

#include <execution>

namespace Utils
{
    static uint32_t FloatToABGR(const glm::vec4& floatColor)
    {
        uint8_t r = (uint8_t)(floatColor.x * 255.0f);
        uint8_t g = (uint8_t)(floatColor.y * 255.0f);
        uint8_t b = (uint8_t)(floatColor.z * 255.0f);
        uint8_t a = (uint8_t)(floatColor.w * 255.0f);

        uint32_t abgr = (a << 24) | (b << 16) | (g << 8) | r;
        return abgr;
    }
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (m_FinalImage) {
        if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height) {
            return;
        }

        m_FinalImage->Resize(width, height);
    } else {
        m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }

    delete[] m_ImageData;
    m_ImageData = new uint32_t[width * height];

    delete[] m_AccumulationData;
    m_AccumulationData = new glm::vec4[width * height];
    m_FrameIndex = 1;

    m_AspectRatio = (float)m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();

    m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIter[i] = i;
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    m_ActiveScene = &scene;
    m_ActiveCamera = &camera;

    if (m_FrameIndex == 1) {
        memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));
    }
    
    if (m_Settings.Multithread) {
        std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
        [this](uint32_t y)
        {
            std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
            [this, y](uint32_t x)
            {
                glm::vec4 floatColor = RayGen(x, y);
                m_AccumulationData[x + (y * m_FinalImage->GetWidth())] += floatColor;
                glm::vec4 accumulatedColor = m_AccumulationData[x + (y * m_FinalImage->GetWidth())] / (float)m_FrameIndex;

                if (m_Settings.Accumulate) {
                    floatColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
                } else {
                    floatColor = glm::clamp(floatColor, glm::vec4(0.0f), glm::vec4(1.0f));
                }
                m_ImageData[x + (y * m_FinalImage->GetWidth())] = Utils::FloatToABGR(floatColor);
            });
        });
    } else {
        for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
        {
            for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
            {
                glm::vec4 floatColor = RayGen(x, y);
                m_AccumulationData[x + (y * m_FinalImage->GetWidth())] += floatColor;
                glm::vec4 accumulatedColor = m_AccumulationData[x + (y * m_FinalImage->GetWidth())] / (float)m_FrameIndex;

                if (m_Settings.Accumulate) {
                    floatColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
                } else {
                    floatColor = glm::clamp(floatColor, glm::vec4(0.0f), glm::vec4(1.0f));
                }
                m_ImageData[x + (y * m_FinalImage->GetWidth())] = Utils::FloatToABGR(floatColor);
            }
        }
    }

    m_FinalImage->SetData(m_ImageData);

    if (m_Settings.Accumulate) {
        m_FrameIndex++;
    } else {
        m_FrameIndex = 1;
    }
}

glm::vec4 Renderer::RayGen(uint32_t x, uint32_t y)
{
    Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

    glm::vec3 color(0.0f);
    float multiplier = 1.0f;
    for (int i = 0; i < m_Settings.Bounces; i++)
    {
        Renderer::HitPayload hit = TraceRay(ray);
        if (hit.HitDistance < 0.0f) {
            color += m_ActiveScene->BackgroundColor * multiplier;
            break;
        }
        const Sphere& hitSphere = m_ActiveScene->Spheres[hit.ObjectIndex];
        const Material& hitMaterial = m_ActiveScene->Materials[hitSphere.MaterialIndex];

        float lightIntensity = glm::max(glm::dot(hit.WorldNormal, - m_ActiveScene->LightDirection), 0.0f);
        glm::vec3 sphereColor = hitMaterial.Albedo * lightIntensity;
        color += sphereColor * multiplier;

        ray.Origin = hit.WorldPosition + hit.WorldNormal * 0.0001f;
        ray.Direction = glm::reflect(ray.Direction, hit.WorldNormal +
                hitMaterial.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
        multiplier *= 0.7f;
    }

    return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
    int closestSphereInd = -1;
    float nearestHitDist = std::numeric_limits<float>::max();
    for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
    {
        const Sphere& sphere = m_ActiveScene->Spheres[i];
        glm::vec3 origin = ray.Origin - sphere.Position;
        float a = glm::dot(ray.Direction, ray.Direction);
        float b = 2.0f * glm::dot(origin, ray.Direction);
        float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;
        float discriminant = b * b - (4.0f * a * c);

        if (discriminant < 0.0f) {
            continue;
        } 

        float rearT = (-b + glm::sqrt(discriminant)) / (2.0f * a);
        float nearT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

        // Make sphere visible from inside
        if (nearT < 0.0f) {
            nearT = rearT;
        }

        if (nearT > 0.0f && nearT < nearestHitDist) {
            nearestHitDist = nearT;
            closestSphereInd = (int)i;
        }
    }

    if (closestSphereInd == -1) {
        return Miss(ray);
    }

    return ClosestHit(ray, nearestHitDist, closestSphereInd);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
    Renderer::HitPayload payload;
    payload.HitDistance = hitDistance;
    payload.ObjectIndex = objectIndex;

    const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];
    glm::vec3 origin = ray.Origin - closestSphere.Position;
    glm::vec3 hitPoint = origin + (ray.Direction * hitDistance);
    payload.WorldNormal = glm::normalize(hitPoint);
    payload.WorldPosition = hitPoint + closestSphere.Position;
    
    return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
    Renderer::HitPayload payload;
    payload.HitDistance = -1.0f;

    return payload;
}
