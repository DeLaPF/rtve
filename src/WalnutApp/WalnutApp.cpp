#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Timer.h"

#include "Camera/Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Walnut::Layer
{
public:
    ExampleLayer()
        : m_Camera(45.0f, 0.1f, 100.0f)
    {
        {
            Material material;
            material.Albedo = { 0.45f, 0.1f, 0.7f };
            material.Roughness = 1.0f;
            material.Metallic = 0.0f;
            m_Scene.Materials.push_back(material);
        }
        {
            Material material;
            material.Albedo = { 0.45f, 0.25f, 0.0f };
            material.Roughness = 1.0f;
            material.Metallic = 0.0f;
            m_Scene.Materials.push_back(material);
        }

        {
            Sphere sphere;
            sphere.Position = { 0.0f, 0.0f, -2.0f };
            sphere.Radius = 1.0f;
            sphere.MaterialIndex = 0;
            m_Scene.Spheres.push_back(sphere);
        }
        {
            Sphere sphere;
            sphere.Position = { 0.0f, -101.0f, -2.0f };
            sphere.Radius = 100.0f;
            sphere.MaterialIndex = 1;
            m_Scene.Spheres.push_back(sphere);
        }

        m_Scene.BackgroundColor = { 0.4f, 0.6f, 0.8f };
    }

	virtual void OnUpdate(float ts) override
	{
		m_Camera.OnUpdate(ts);
	}

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

        ImGui::End();

        ImGui::Begin("Scene");

        ImGui::Text("Spheres");
        for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
        {
            ImGui::PushID(i);

            Sphere& sphere = m_Scene.Spheres[i];
            ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
            ImGui::DragFloat("Radius", &sphere.Radius, 0.1f, 0.1f, std::numeric_limits<float>::max());
            ImGui::DragInt("Material Index", &sphere.MaterialIndex, 0, m_Scene.Materials.size() - 1);
            ImGui::Separator();

            ImGui::PopID();
        }


        ImGui::Text("Materials");
        for (size_t i = 0; i < m_Scene.Materials.size(); i++)
        {
            ImGui::PushID(i);

            Material& material = m_Scene.Materials[i];
            ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
            ImGui::DragFloat("Roughness", &material.Roughness, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Metallic", &material.Roughness, 0.01f, 0.0f, 1.0f);
            ImGui::Separator();

            ImGui::PopID();
        }
        
        ImGui::Text("Light");
        ImGui::DragFloat3("Light Direction", glm::value_ptr(m_Scene.LightDirection), 0.01f, -1.0f, 1.0f);

        ImGui::Text("Background");
        ImGui::ColorEdit4("Color", glm::value_ptr(m_Scene.BackgroundColor));

		ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

        m_ViewportWidth = ImGui::GetContentRegionAvail().x;
        m_ViewportHeight = ImGui::GetContentRegionAvail().y;

        auto image = m_Renderer.GetFinalImage();
        if (image) {
            ImGui::Image(image->GetDescriptorSet(),
                    { (float)image->GetWidth(), (float)image->GetHeight() },
                    ImVec2(0, 1), ImVec2(1, 0));
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

        m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

        m_LastRenderTime = timer.ElapsedMillis();
    }

private:
    Renderer m_Renderer;
    Camera m_Camera;
    Scene m_Scene;

    bool m_TogglePause = false;
    float m_LastRenderTime = 0.0f;

    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
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
