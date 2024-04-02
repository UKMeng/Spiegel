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
	{
		SPG_PROFILE_SCOPE("Renderer Preperation");
		spg::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		spg::RenderCommand::Clear();
	}
	

	{
		SPG_PROFILE_SCOPE("Renderer Draw");
		spg::Renderer2D::BeginScene(m_CameraController.GetCamera());
		spg::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		spg::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		spg::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
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
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::ShowDemoWindow();

	m_ProfileResults.clear();

	ImGui::End();
}
