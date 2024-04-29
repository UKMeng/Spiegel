#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace spg {

	// TODO: Move to Core ?
	enum class BlendFactor {
		Zero, One, SrcColor, OneMinusSrcColor, DstColor, OneMinusDstColor, SrcAlpha, OneMinusSrcAlpha, DstAlpha, OneMinusDstAlpha, ConstantColor, OneMinusConstantColor, ConstantAlpha, OneMinusConstantAlpha
	};

	enum class CompareFunc {
		Always, Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual,
	};

	enum class StencilOp {
		Keep, Zero, Replace, Increment, IncrementWrap, Decrement, DecrementWrap, Invert
	};

	enum class CullFace {
		Front, Back, FrontAndBack
	};

	class RendererAPI {
	public:
		enum class API {
			None = 0, OpenGL = 1
		};

	public:
		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
		
		virtual void SetLineWidth(float width) = 0;

		virtual void SetWireframeMode(bool flag) = 0;

		// Blending
		virtual void SetBlend(bool flag) = 0;
		virtual void SetBlendFunc(BlendFactor sfactor, BlendFactor dfactor) = 0;
		virtual void SetBlendSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha) = 0;

		// Depth Test
		virtual void SetDepthTest(bool flag) = 0;
		virtual void SetDepthMask(bool flag) = 0;
		virtual void SetDepthFunc(CompareFunc func) = 0;

		// Stencil Test
		virtual void SetStencilTest(bool flag) = 0;
		virtual void SetStencilMask(uint32_t mask) = 0;
		virtual void SetStencilFunc(CompareFunc func, int ref, uint32_t mask) = 0;
		virtual void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) = 0;

		// Face Culling
		virtual void SetCullFace(bool flag) = 0;
		virtual void SetCullFaceSide(CullFace side) = 0;
		// 0 = Clockwise, 1 = Counter-Clockwise
		virtual void SetCullFaceFrontFace(int mode) = 0;


		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}