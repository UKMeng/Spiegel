#pragma once

#include "Texture.h"

#include <glm/glm.hpp>

namespace spg
{
	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, glm::vec2& max);

		const Ref<Texture2D> GetTexture() const { return m_Texture; }
		const glm::vec2* GetTexCoords() const { return m_TexCoords; }

		/// @brief Create a subtexture from a texture and a bounding box
		/// @param texture The texture to create the subtexture from
		/// @param coords The coordinates of the bounding box
		/// @param cellSize The standard size of a cell in the texture
		/// @param spriteSize The subtexture size relative to the cell size
		/// @return a Ref to the created subtexture
		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1, 1 });
	private:
		Ref<Texture2D> m_Texture;

		glm::vec2 m_TexCoords[4];
	};
}