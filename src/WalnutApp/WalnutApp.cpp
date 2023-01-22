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
