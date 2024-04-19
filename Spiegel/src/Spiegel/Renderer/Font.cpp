#include "spgpch.h"
#include "Font.h"

namespace spg {

	Font::Font(const std::filesystem::path& fontPath, uint32_t fontSize)
		: m_fontPath(fontPath), m_fontSize(fontSize)
	{
		// SPG_CORE_WARN("Font created: {0}, size: {1}", m_fontPath.string(), m_fontSize);
		LoadBasicAscii();
		m_isDefault = false;
	}

	bool Font::Attach()
	{
		if (FT_Init_FreeType(&m_ftLibrary)) {
			SPG_CORE_ERROR("Failed to initialize FreeType library");
		}

		if (FT_New_Face(m_ftLibrary, m_fontPath.string().c_str(), 0, &m_ftFace)) {
			SPG_CORE_ERROR("Failed to load font: {0}", m_fontPath.string());
		}

		FT_Set_Pixel_Sizes(m_ftFace, 0, m_fontSize);

		return true;
	}

	bool Font::Detach()
	{
		FT_Done_Face(m_ftFace);
		FT_Done_FreeType(m_ftLibrary);
		return true;
	}

	bool Font::CharacterLoader(std::wstring text)
	{
		for (wchar_t c : text) {
			if (m_characters.find(c) == m_characters.end()) {
				Attach();
				if (!LoadCharacter(c)) {
					SPG_CORE_ERROR("Failed to load character: {0}", (char)c);
					return false;
				}
				Detach();
			}
		}
		return true;
	}

	Ref<Font> Font::GetDefaultFont()
	{
		static Ref<Font> defaultFont;
		if (!defaultFont) {
			defaultFont = CreateRef<Font>("assets/fonts/Noto_Sans/NotoSansSC-Bold.ttf", 200);
			defaultFont->m_isDefault = true;
		}
		return defaultFont;
	}

	void Font::ReloadFont()
	{
		m_characters.clear();
		LoadBasicAscii();
		m_isDefault = false;
	}

	bool Font::LoadBasicAscii()
	{
		Attach();
		for (int i = 0; i < 128; i++) {
			wchar_t c = static_cast<wchar_t>(i);
			if (!LoadCharacter(c)) {
				SPG_CORE_ERROR("Failed to load character: {0}", char(c));
				return false;
			}
		}
		Detach();
		return true;
	}

	bool Font::LoadCharacter(wchar_t c)
	{
		if (FT_Load_Char(m_ftFace, c, FT_LOAD_RENDER)) {
			SPG_CORE_ERROR("Failed to load Glyph: {0}", (char)c);
			return false;
		}

		Character character = {
			Texture2D::Create(m_ftFace),
			glm::ivec2(m_ftFace->glyph->bitmap.width, m_ftFace->glyph->bitmap.rows),
			glm::ivec2(m_ftFace->glyph->bitmap_left, m_ftFace->glyph->bitmap_top),
			static_cast<uint32_t>(m_ftFace->glyph->advance.x)
		};

		m_characters.insert(std::pair<wchar_t, Character>(c, character));
		return true;
	}
}