#pragma once

#include "Texture.h"

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace spg {

	// 中文汉字的字体如果要实时渲染每个字相当消耗性能，
	// 未来的计划应该是提前获取游戏所需要的所有文本，然后一次性渲染到一个大的纹理上
	// 然后在游戏中直接使用纹理以及定位来渲染字体

	// TODO：MSDF also need to be implemented
	// https://github.com/Chlumsky/msdf-atlas-gen
	// https://www.youtube.com/watch?v=J26hm7r-k6A
	
	// FreeType Font
	// https://learnopengl.com/In-Practice/Text-Rendering

	struct Character
	{
		Ref<Texture2D> Texture;
		glm::ivec2 Size;
		glm::ivec2 Bearing;
		uint32_t Advance;
	};
	
	class Font
	{
	public:
		Font() = default;
		Font(const std::filesystem::path& fontPath, uint32_t fontSize);
		~Font() = default;
		
		bool Attach();
		bool Detach();
		bool CharacterLoader(std::wstring text);

		std::string GetFontPath() const { return m_fontPath.string(); }

		uint32_t GetFontSize() const { return m_fontSize; }
		void SetFontSize(uint32_t fontSize) { m_fontSize = fontSize; ReloadFont(); }

		Character GetCharacter(wchar_t c) { return m_characters[c]; }

		bool IsDefault() const { return m_isDefault; }

		static Ref<Font> GetDefaultFont();
	private:
		void ReloadFont();
		bool LoadBasicAscii();
		bool LoadCharacter(wchar_t c);
	private:
		std::filesystem::path m_fontPath;
		bool m_isDefault = false;
		uint32_t m_fontSize;
		std::unordered_map<wchar_t, Character> m_characters;
		FT_Library m_ftLibrary;
		FT_Face m_ftFace;
	};


}