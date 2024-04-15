#include "spgpch.h"
#include "Font.h"

namespace spg {

	Font::Font(const std::filesystem::path& fontPath, uint32_t fontSize)
		: m_fontPath(fontPath), m_fontSize(fontSize)
	{
		if (FT_Init_FreeType(&m_ftLibrary)) {
			SPG_CORE_ERROR("Failed to initialize FreeType library");
		}
	}
}