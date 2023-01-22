#include "Renderer.h"
#include <cstdint>

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
            //m_ImageData[x + (y * m_FinalImage->GetWidth())] = PerPixel(coord);
            m_ImageData[x + (y * m_FinalImage->GetWidth())] = DrawSphere(coord);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
    uint8_t r = (uint8_t)(coord.x * 255.0f);
    uint8_t g = (uint8_t)(coord.y * 255.0f);
    
    return 0xff000000 | (g << 8) | r;
}

uint32_t Renderer::DrawSphere(glm::vec2 coord)
{
    glm::vec3 rayDirection = glm::vec3(coord.x + m_Camera.x, coord.y + m_Camera.y, m_Camera.z + m_CameraFocalLength);
    glm::vec3 rayOrigin = m_Camera, sphereOrigin = m_Sphere;
    float radius = m_SphereRadius;

    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(rayOrigin, rayDirection) - 2.0f * glm::dot(rayDirection, sphereOrigin);
    float c = glm::dot(rayOrigin, rayOrigin) - 2.0f * glm::dot(rayOrigin, sphereOrigin) + glm::dot(sphereOrigin, sphereOrigin) - radius * radius;
    float determinant = b * b - (4.0f * a * c);

    if (determinant >= 0.0f) {
        return 0xffff00ff;
    } 

    return 0xff000000;
}
