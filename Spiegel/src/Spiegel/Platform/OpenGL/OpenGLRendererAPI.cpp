#include "spgpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace spg {

	namespace Utils {
		static GLenum BlendFactorToGL(BlendFactor factor)
		{
			switch (factor)
			{
			case BlendFactor::Zero: return GL_ZERO;
			case BlendFactor::One: return GL_ONE;
			case BlendFactor::SrcColor: return GL_SRC_COLOR;
			case BlendFactor::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
			case BlendFactor::DstColor: return GL_DST_COLOR;
			case BlendFactor::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
			case BlendFactor::SrcAlpha: return GL_SRC_ALPHA;
			case BlendFactor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
			case BlendFactor::DstAlpha: return GL_DST_ALPHA;
			case BlendFactor::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
			case BlendFactor::ConstantColor: return GL_CONSTANT_COLOR;
			case BlendFactor::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
			case BlendFactor::ConstantAlpha: return GL_CONSTANT_ALPHA;
			case BlendFactor::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
			}

			SPG_CORE_ASSERT(false, "Unknown BlendFactor");
			return 0;
		}

		static GLenum CompareFuncToGL(CompareFunc func)
		{
			switch (func)
			{
			case CompareFunc::Always: return GL_ALWAYS;
			case CompareFunc::Never: return GL_NEVER;
			case CompareFunc::Less: return GL_LESS;
			case CompareFunc::Equal: return GL_EQUAL;
			case CompareFunc::LessEqual: return GL_LEQUAL;
			case CompareFunc::Greater: return GL_GREATER;
			case CompareFunc::GreaterEqual: return GL_GEQUAL;
			case CompareFunc::NotEqual: return GL_NOTEQUAL;
			}

			SPG_CORE_ASSERT(false, "Unknown CompareFunc");
			return 0;
		}

		static GLenum StencilOpToGL(StencilOp op)
		{
			switch (op)
			{
			case StencilOp::Keep: return GL_KEEP;
			case StencilOp::Zero: return GL_ZERO;
			case StencilOp::Replace: return GL_REPLACE;
			case StencilOp::Increment: return GL_INCR;
			case StencilOp::IncrementWrap: return GL_INCR_WRAP;
			case StencilOp::Decrement: return GL_DECR;
			case StencilOp::DecrementWrap: return GL_DECR_WRAP;
			case StencilOp::Invert: return GL_INVERT;
			}

			SPG_CORE_ASSERT(false, "Unknown StencilOp");
			return 0;
		}

		static GLenum CullFaceToGL(CullFace face)
		{
			switch (face)
			{
			case CullFace::Front: return GL_FRONT;
			case CullFace::Back: return GL_BACK;
			case CullFace::FrontAndBack: return GL_FRONT_AND_BACK;
			}

			SPG_CORE_ASSERT(false, "Unknown CullFace");
			return 0;
		}
	}

	void OpenGLRendererAPI::Init() {
		// setup blend function
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) {
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) {
		// TODO: Maybe bug? to fix
		// uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		// glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) {
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	/*void OpenGLRendererAPI::DrawCubes(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) {
		vertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}*/

	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		// globally set line width
		glLineWidth(width);
	}

	void OpenGLRendererAPI::SetWireframeMode(bool flag)
	{
		if (flag)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void OpenGLRendererAPI::SetBlend(bool flag)
	{
		if (flag) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else {
			glDisable(GL_BLEND);
		}
	}

	void OpenGLRendererAPI::SetBlendFunc(BlendFactor sfactor, BlendFactor dfactor)
	{
		glBlendFunc(Utils::BlendFactorToGL(sfactor), Utils::BlendFactorToGL(dfactor));
	}

	void OpenGLRendererAPI::SetBlendSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha)
	{
		glBlendFuncSeparate(Utils::BlendFactorToGL(srcRGB), Utils::BlendFactorToGL(dstRGB), Utils::BlendFactorToGL(srcAlpha), Utils::BlendFactorToGL(dstAlpha));
	}

	void OpenGLRendererAPI::SetDepthTest(bool flag)
	{
		if (flag)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetDepthMask(bool flag)
	{
		if (flag)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
	}

	void OpenGLRendererAPI::SetDepthFunc(CompareFunc func)
	{
		glDepthFunc(Utils::CompareFuncToGL(func));
	}

	void OpenGLRendererAPI::SetStencilTest(bool flag)
	{
		if (flag)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	
	}
	void OpenGLRendererAPI::SetStencilMask(uint32_t mask)
	{
		glStencilMask(mask);
	}

	void OpenGLRendererAPI::SetStencilFunc(CompareFunc func, int ref, uint32_t mask)
	{
		glStencilFunc(Utils::CompareFuncToGL(func), ref, mask);
	}

	void OpenGLRendererAPI::SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass)
	{
		glStencilOp(Utils::StencilOpToGL(sfail), Utils::StencilOpToGL(dpfail), Utils::StencilOpToGL(dppass));
	}

	void OpenGLRendererAPI::SetCullFace(bool flag)
	{
		if (flag) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
			
	}

	void OpenGLRendererAPI::SetCullFaceSide(CullFace side)
	{
		glCullFace(Utils::CullFaceToGL(side));
	}

	void OpenGLRendererAPI::SetCullFaceFrontFace(int mode)
	{
		switch (mode) {
		case 0: glFrontFace(GL_CW); break;
		case 1: glFrontFace(GL_CCW); break;
		}
	}
}