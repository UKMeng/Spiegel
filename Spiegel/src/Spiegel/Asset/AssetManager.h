#pragma once

#include "Asset.h"

namespace spg {

	class AssetManager {
	public:

		struct AssetMetadata {
			UUID AssetHandle;
			AssetType Type;
			std::string FilePath;
		};

		static void Init();
		static void Shutdown();

	private:
		static std::unordered_map<UUID, Ref<Asset>> s_LoadedAssets;
	};
}