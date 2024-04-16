#pragma once

#include "Texture.h"

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace spg {

	// ���ĺ��ֵ��������Ҫʵʱ��Ⱦÿ�����൱�������ܣ�
	// δ���ļƻ�Ӧ������ǰ��ȡ��Ϸ����Ҫ�������ı���Ȼ��һ������Ⱦ��һ�����������
	// Ȼ������Ϸ��ֱ��ʹ�������Լ���λ����Ⱦ����

	// TODO��MSDF also need to be implemented
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

		Character GetCharacter(wchar_t c) { return m_characters[c]; }

		static Ref<Font> GetDefaultFont();
	private:
		bool LoadCharacter(wchar_t c);
	private:
		std::filesystem::path m_fontPath;
		uint32_t m_fontSize;
		std::unordered_map<wchar_t, Character> m_characters;
		FT_Library m_ftLibrary;
		FT_Face m_ftFace;
	};


}