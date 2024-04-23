#pragma once

#include "Spiegel/Core/UUID.h"

#include <string>

namespace spg {

	enum class AssetType {
		None = 0,
		Texture,
		Font,
		Script,
		Scene,
		Directory
	};

	class Asset
	{
	public:
		virtual ~Asset() = default;

		UUID AssetHandle;
		AssetType Type = AssetType::None;
		std::string FilePath;
		std::string FileName;
		std::string Extension;
		bool IsLoaded = false;
		
	};
}