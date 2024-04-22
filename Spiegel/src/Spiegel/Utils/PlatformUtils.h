#pragma once

#include <string>
#include <Windows.h>

namespace spg {

	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}