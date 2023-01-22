#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");

        ImGui::Text("Last Render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render Single")) {
            Render();
        }
		if (ImGui::Button("Toggle Pause")) {
            m_TogglePause = !m_TogglePause;
        }

        ImGui::Text("Focal Length: %.3fm", m_Camera.w);
		if (ImGui::Button("Decrease Focal Length")) {
            m_Camera = ImVec4(m_Camera.x, m_Camera.y, m_Camera.z, m_Camera.w - 0.05f);
        }
		if (ImGui::Button("Increase Focal Length")) {
            m_Camera = ImVec4(m_Camera.x, m_Camera.y, m_Camera.z, m_Camera.w + 0.05f);
        }

        ImGui::Text("Sphere Radius: %.3fm", m_Sphere.w);
		if (ImGui::Button("Decrease Sphere Radius")) {
            m_Sphere = ImVec4(m_Sphere.x, m_Sphere.y, m_Sphere.z, m_Sphere.w - 0.05f);
        }
		if (ImGui::Button("Increase Sphere Radius")) {
            m_Sphere = ImVec4(m_Sphere.x, m_Sphere.y, m_Sphere.z, m_Sphere.w + 0.05f);
        }

		ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

        m_ViewportWidth = ImGui::GetContentRegionAvail().x;
        m_ViewportHeight = ImGui::GetContentRegionAvail().y;

        if (m_Image) {
            ImGui::Image(m_Image->GetDescriptorSet(), { (float)m_Image->GetWidth(), (float)m_Image->GetHeight() });
        }

		ImGui::End();
        ImGui::PopStyleVar();

        if (!m_TogglePause) {
            Render();
        }
	}

    void Render()
    {
        Walnut::Timer timer;

        if (!m_Image || m_ViewportWidth != m_Image->GetWidth() || m_ViewportHeight != m_Image->GetHeight()) {
            m_Image = std::make_shared<Walnut::Image>(m_ViewportWidth, m_ViewportHeight, Walnut::ImageFormat::RGBA);
            delete[] m_ImageData;
            m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
        }

        for (uint32_t i = 0; i < m_ViewportWidth * m_ViewportHeight; i++)
        {
            DrawSphere(m_ImageData, i);
        }
        m_Image->SetData(m_ImageData);

        m_LastRenderTime = timer.ElapsedMillis();
    }

private:
    bool m_TogglePause = true;
    float m_LastRenderTime = 0.0f;

    std::shared_ptr<Walnut::Image> m_Image;
    uint32_t* m_ImageData = nullptr;
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

    // x, y, z, 'w' is focalLength
    ImVec4 m_Camera = ImVec4(0.0f, 0.0f, -4.0f, 1.0f); 

    // x, y, z, 'w' is raidus
    ImVec4 m_Sphere = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    void DrawSphere(uint32_t* imageData, uint32_t index)
    {
        uint32_t pixelX = index % m_ViewportWidth, pixelY = index / m_ViewportWidth;
        // map from -1 to 1 on the x (left to right) and y (top to bottom)
        float x = ((float)pixelX / (float)m_ViewportWidth) * 2.0f - 1.0f, y = ((float)pixelY / (float)m_ViewportHeight) * 2.0f - 1.0f;

        ImVec4 direction = ImVec4(x + m_Camera.x, y + m_Camera.y, m_Camera.z + m_Camera.w, 0.0f);
        if (DoesRayHitSphere(&m_Camera, &direction, &m_Sphere, m_Sphere.w)) {
            m_ImageData[index] = 0xffff00ff;
        } else {
            m_ImageData[index] = 0xff000000;
        }
    }

    bool DoesRayHitSphere(ImVec4* rayOrigin, ImVec4* direction, ImVec4* sphereOrigin, float radius)
    {
        // rayOrigin: the location of the origin of the ray (the camera location)
        // direction: the normalized direction of the ray
        // (x,y from pixel location and z from the camera + focal length)
        // Note: this limits the camera to only looking forward presently
        // sphereOrigin: the location of the origin of the sphere
        // radius: the radius of the sphere

        // ray: a + bt
        // a_x + b_xt
        // a_y + b_yt
        // a_z + b_zt
        // sphere: (x-c)^2 + (y-d)^2 + (z-e)^2 - r^2 = 0
        // note:
        // (a_x, a_y, a_z) is the origin of the ray
        // (b_x, b_y, b_z) is the direction of the ray
        // 't' is the distace to travel along the ray before intersection
        // (c, d, e) is the origin of the sphere)
        // 'r' is the radius of the sphere

        // find values for 't' (if they exist) such that:
        // (a_x + b_xt - c)^2 + (a_y + b_yt - d)^2 + (a_z + b_zt - e)^2 
        // expand above into quadratic form, with coefficients relative to 't'
        // as follows:
        // a = (b_x^2 + b_y^2 + b_z^2)
        // b = (2a_xb_x - 2b_xc + 2a_yb_y - 2b_yd + 2a_zb_z - 2b_ze)
        // c = (a_x^2 - 2a_xc + c^2 + a_y^2 - 2a_yd + d^2 + a_z^2 - 2a_ze + e^2 - r^2)
        //
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
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Traced Vulkan Engine";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}
