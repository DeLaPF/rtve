#include "Renderer.h"
#include <cstdint>

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
    m_AspectRatio = (float)m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    Ray ray;
	ray.Origin = camera.GetPosition();
    
    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];
			glm::vec4 floatColor = TraceRay(scene, ray);
            floatColor = glm::clamp(floatColor, glm::vec4(0.0f), glm::vec4(1.0f)); 
            m_ImageData[x + (y * m_FinalImage->GetWidth())] = Utils::FloatToABGR(floatColor);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
    const Sphere* closestSphere = nullptr;
    float nearestHitDist = std::numeric_limits<float>::max();
    for (const Sphere& sphere : scene.Spheres)
    {
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
            closestSphere = &sphere;
        }
    }

    if (closestSphere == nullptr) {
        return glm::vec4(scene.BackgroundColor);
    }

    glm::vec3 origin = ray.Origin - closestSphere->Position;
    glm::vec3 hitPoint = origin + (ray.Direction * nearestHitDist);
    glm::vec3 surfaceNormal = glm::normalize(hitPoint);
    //glm::vec3 surfaceNormal = glm::normalize(hitPoint - closestSphere->Position);
    float lightIntensity = glm::max(glm::dot(surfaceNormal, - scene.LightDirection), 0.0f);

    glm::vec3 sphereColor = closestSphere->Albedo * lightIntensity;
    return glm::vec4(sphereColor, closestSphere->Opacity);
}
