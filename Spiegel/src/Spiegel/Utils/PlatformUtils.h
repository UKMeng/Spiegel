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

	class PlatformUtils
	{
	public:
		static std::wstring string2Wstring(const std::string& str) {
			if (str.size() == 0) return L"";
			std::wstring result;
			int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
			TCHAR* buffer = new TCHAR[len + 1];
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
			buffer[len] = '\0';
			result.append(buffer);
			delete[] buffer;
			return result;
		}

	};
}