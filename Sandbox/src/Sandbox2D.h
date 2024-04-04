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
	spg::OrthographicCameraController m_CameraController;

	spg::Ref<spg::Texture2D> m_CheckerboardTexture;

	spg::Ref<spg::Texture2D> m_SpriteSheet;
	spg::Ref<spg::SubTexture2D> m_TextureStairs, m_TextureTree;

	struct ProfileResult {
		const char* Name;
		float Time;
	};
	std::vector<ProfileResult> m_ProfileResults;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	// Particle
	ParticleProps m_Particle;
	ParticleSystem m_ParticleSystem;

	size_t m_MapWidth, m_MapHeight;
	std::unordered_map<char, spg::Ref<spg::SubTexture2D>> m_TextureMap;
};