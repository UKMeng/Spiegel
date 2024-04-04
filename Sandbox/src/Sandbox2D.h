#pragma once

#include "Spiegel.h"

#include "ParticleSystem.h"

class Sandbox2D: public spg::Layer {
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(spg::Timestep ts) override;
	virtual void OnEvent(spg::Event& e) override;
	virtual void OnImGuiRender() override;

private:
	spg::Ref<spg::Shader> m_FlatColorShader;
	spg::Ref<spg::Texture2D> m_Texture;
	spg::Ref<spg::VertexArray> m_SquareVA;
	spg::OrthographicCameraController m_CameraController;

	spg::Ref<spg::Texture2D> m_CheckerboardTexture;

	struct ProfileResult {
		const char* Name;
		float Time;
	};

	std::vector<ProfileResult> m_ProfileResults;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	// Particle
	ParticleProps m_Particle;
	ParticleSystem m_ParticleSystem;
};