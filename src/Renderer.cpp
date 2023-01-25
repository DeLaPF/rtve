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

void Renderer::Render()
{
    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec2 coord = { (x / (float)m_FinalImage->GetWidth()) * m_AspectRatio,
                                 y / (float)m_FinalImage->GetHeight() };
            coord = coord * 2.0f - 1.0f; // map from -1 to 1
            //glm::vec4 floatColor = PerPixel(coord);
            glm::vec4 floatColor = DrawSphere(coord);
            floatColor = glm::clamp(floatColor, glm::vec4(0.0f), glm::vec4(1.0f)); 
            m_ImageData[x + (y * m_FinalImage->GetWidth())] = Utils::FloatToABGR(floatColor);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
    coord = coord * 0.5f + 0.5f; // map from 0 to 1
    return glm::vec4(coord, 0.0f, 1.0f);
}

glm::vec4 Renderer::DrawSphere(glm::vec2 coord)
{
    glm::vec3 rayDirection = glm::vec3(coord.x, coord.y, m_CameraFocalLength);
    //rayDirection = glm::normalize(rayDirection);
    glm::vec3 rayOrigin = m_Camera, sphereOrigin = m_Sphere;
    float radius = m_SphereRadius;

    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(rayOrigin, rayDirection) - 2.0f * glm::dot(rayDirection, sphereOrigin);
    float c = glm::dot(rayOrigin, rayOrigin) - 2.0f * glm::dot(rayOrigin, sphereOrigin) + glm::dot(sphereOrigin, sphereOrigin) - radius * radius;
    float determinant = b * b - (4.0f * a * c);

    if (determinant < 0.0f) {
        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    } 

    //float t0 = (-b + glm::sqrt(determinant)) / (2.0f * a);
    float t1 = (-b - glm::sqrt(determinant)) / (2.0f * a);
    
    //glm::vec3 farHit = rayOrigin + (rayDirection * t0);
    glm::vec3 nearHit = rayOrigin + (rayDirection * t1);

    glm::vec3 surfaceNormal = glm::normalize(nearHit - sphereOrigin);
    glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    float lightIntensity = glm::max(glm::dot(surfaceNormal, -lightDirection), 0.0f);

    glm::vec3 sphereColor = glm::vec3(1.0f, 0.0f, 1.0f) * lightIntensity;
    return glm::vec4(sphereColor, 1.0f);
}
