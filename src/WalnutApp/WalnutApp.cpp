#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer.h"

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

        ImGui::Text("Set Sphere Color");
        static float r = 1.0f, g = 0.0f, b = 1.0f, a = 1.0f;
        ImGui::SliderFloat("R", &r, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("G", &g, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("B", &b, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("A", &a, 0.0f, 1.0f, "%.3f");
        m_Renderer.SetSphereColor(glm::vec4(r, g, b, a));

        ImGui::Text("Set Sphere Location");
        static float sphereX = 0.0f, sphereY = 0.0f, sphereZ = 0.0f;
        ImGui::SliderFloat("Sphere X", &sphereX, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Sphere Y", &sphereY, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Sphere Z", &sphereZ, -1.0f, 1.0f, "%.3f");
        m_Renderer.SetSphereLocation(glm::vec3(sphereX, sphereY, sphereZ));
        
        ImGui::Text("Set Light Direction");
        static float lightX = -1.0f, lightY = -1.0f, lightZ = -1.0f;
        ImGui::SliderFloat("Light X", &lightX, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Light Y", &lightY, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Light Z", &lightZ, -1.0f, 1.0f, "%.3f");
        m_Renderer.SetLightDirection(glm::vec3(lightX, lightY, lightZ));

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
        m_Renderer.Render();

        m_LastRenderTime = timer.ElapsedMillis();
    }

private:
    Renderer m_Renderer;
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
