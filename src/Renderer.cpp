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
    glm::vec3 direction = glm::vec3(coord.x + m_Camera.x, coord.y + m_Camera.y, m_Camera.z + m_Camera.w);
    if (DoesRayHitSphere(&m_Camera, &direction, &m_Sphere, m_Sphere.w)) {
        return 0xffff00ff;
    } 

    return 0xff000000;
}

bool Renderer::DoesRayHitSphere(glm::vec4* rayOrigin, glm::vec3* direction, glm::vec4* sphereOrigin, float radius)
{
    // find values for 't' (if they exist) such that:
    // (a_x + b_xt - c)^2 + (a_y + b_yt - d)^2 + (a_z + b_zt - e)^2 - r^2 = 0
    // expand above into quadratic form, with coefficients relative to 't'
    // as follows:
    // a = (b_x^2 + b_y^2 + b_z^2)
    // b = (2a_xb_x - 2b_xc + 2a_yb_y - 2b_yd + 2a_zb_z - 2b_ze)
    // c = (a_x^2 - 2a_xc + c^2 + a_y^2 - 2a_yd + d^2 + a_z^2 - 2a_ze + e^2 - r^2)
    // sovle for determinant: b^2 - 4ac to see if there is an intersection
    float rX = rayOrigin->x, rY = rayOrigin->y, rZ = rayOrigin->z;
    float dX = direction->x, dY = direction->y, dZ = direction->z;
    float sX = sphereOrigin->x, sY = sphereOrigin->y, sZ = sphereOrigin->z;
    float a = (dX * dX) + (dY * dY) + (dZ * dZ);
    float b = ((2 * rX * dX) - (2 * dX * sX)) + ((2 * rY * dY) - (2 * dY * sY)) + ((2 * rZ * dZ) - (2 * dZ * sZ));
    float c = ((rX * rX) - (2 * rX * sX) + (sX * sX)) + ((rY * rY) - (2 * rY * sY) + (sY * sY)) + ((rZ * rZ) - (2 * rZ * sZ) + (sZ * sZ)) - (radius * radius);
    float determinant = b * b - (4 * a * c);

    return determinant >= 0.0f;
}
