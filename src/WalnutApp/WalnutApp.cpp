#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Camera/Camera.h"
#include "Renderer.h"
#include "Scene.h"

#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Walnut::Layer
{
public:
    ExampleLayer()
        : m_Camera(45.0f, 0.1f, 100.0f)
    {
        {
            Sphere sphere;
            sphere.Position = { 0.0f, 0.0f, 0.0f };
            sphere.Radius = 0.5f;
            sphere.Albedo = { 1.0f, 1.0f, 1.0f };
            sphere.Opacity = 1.0f;
            m_Scene.Spheres.push_back(sphere);
        }

        {
            Sphere sphere;
            sphere.Position = { 1.0f, 0.5f, -2.0f };
            sphere.Radius = 1.0f;
            sphere.Albedo = { 1.0f, 0.0f, 1.0f };
            sphere.Opacity = 1.0f;
            m_Scene.Spheres.push_back(sphere);
        }
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
            ImGui::ColorEdit3("Albedo", glm::value_ptr(sphere.Albedo), 0.1f);
            ImGui::DragFloat("Opacity", &sphere.Opacity, 0.01f, 0.0f, 1.0f);
            ImGui::Separator();

            ImGui::PopID();
        }
        
        ImGui::Text("Light");
        ImGui::DragFloat3("Light Direction", glm::value_ptr(m_Scene.LightDirection), 0.01f, -1.0f, 1.0f);

        ImGui::Text("Background");
        ImGui::ColorEdit4("Color", glm::value_ptr(m_Scene.BackgroundColor), 0.01f);

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
