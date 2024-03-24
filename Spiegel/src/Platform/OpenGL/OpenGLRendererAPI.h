#pragma once

#include "Spiegel/Renderer/RendererAPI.h"

namespace spg {
	class OpenGLRendererAPI : public RendererAPI {
		virtual void Init() override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	};
}