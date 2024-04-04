#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{

}

void Sandbox2D::OnAttach()
{
	SPG_PROFILE_FUNCTION();

	m_CheckerboardTexture = spg::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	SPG_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(spg::Timestep ts)
{
	SPG_PROFILE_FUNCTION();
	// Camera
	m_CameraController.OnUpdate(ts);


	// Renderer
	spg::Renderer2D::ResetStats();

	{
		SPG_PROFILE_SCOPE("Renderer Preperation");
		spg::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		spg::RenderCommand::Clear();
	}
	

	{
		static float rotation = 0.0f;
		rotation += ts * 20.0f;

		SPG_PROFILE_SCOPE("Renderer Draw");
		spg::Renderer2D::BeginScene(m_CameraController.GetCamera());
		spg::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, rotation, { 0.8f, 0.2f, 0.3f, 1.0f });
		spg::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		spg::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		spg::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f}, {20.0f, 20.0f}, m_CheckerboardTexture, 10.0f, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
		spg::Renderer2D::DrawRotatedQuad({ -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, 45.0f, m_CheckerboardTexture, 20.0f);
		spg::Renderer2D::EndScene();

		spg::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f) {
			for (float x = -5.0f; x < 5.0f; x += 0.5f) {
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f };
				spg::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		spg::Renderer2D::EndScene();
	}

	
}

void Sandbox2D::OnEvent(spg::Event& e)
{
	m_CameraController.OnEvent(e);
}


void Sandbox2D::OnImGuiRender()
{
	SPG_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	
	// Renderer2D Stats
	auto stats = spg::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::ShowDemoWindow();

	m_ProfileResults.clear();

	ImGui::End();
}
