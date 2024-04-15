#pragma once

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
	
	class Font
	{
	public:
		Font() = default;
		Font(const std::filesystem::path& fontPath, uint32_t fontSize);
		~Font() = default;
	private:
		std::filesystem::path m_fontPath;
		uint32_t m_fontSize;
		FT_Library m_ftLibrary;
	};


}