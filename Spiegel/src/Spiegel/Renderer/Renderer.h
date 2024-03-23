#pragma once

namespace spg {
	enum class RendererAPI {
		None = 0, OpenGL = 1
	};

	class Renderer {
		// store the static state of the renderer API
	public:
		inline static RendererAPI GetAPI() { return s_RendererAPI; }
	private:
		static RendererAPI s_RendererAPI;
	};
}