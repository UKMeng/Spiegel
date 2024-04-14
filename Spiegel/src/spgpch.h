#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <array>
#include <functional>
#include <sstream>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>


#ifdef SPG_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#include "Spiegel/Core/Log.h"
#include "Spiegel/Debug/Instrumentation.h"