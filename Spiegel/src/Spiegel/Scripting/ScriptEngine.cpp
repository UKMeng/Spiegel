#include "spgpch.h"
#include "ScriptEngine.h"

namespace spg {

	struct ScriptEngineData
	{
        HMODULE hmod;
	};

	static ScriptEngineData* s_Data;

	void ScriptEngine::Init()
	{
        s_Data = new ScriptEngineData();

        s_Data->hmod = LoadLibrary(L"assets/scripts/ExampleGame.dll");
        if (s_Data->hmod == NULL)
        {
            DWORD dw = GetLastError();
            printf("Failed to load DLL: error code %d\n", dw);
            return;
        }
        return;
	}

	void ScriptEngine::Shutdown()
	{
        if (!FreeLibrary(s_Data->hmod))
        {
            DWORD dw = GetLastError();
            printf("Failed to free DLL: error code %d\n", dw);
            return;
        }
        delete s_Data;
	}

    ScriptFunc ScriptEngine::GetInstantiateScript(const char* funcName)
    {
        ScriptFunc func = (ScriptFunc)GetProcAddress(s_Data->hmod, funcName);
        if (func == NULL)
        {
            DWORD dw = GetLastError();
            printf("Failed to get function: error code %d\n", dw);
            return nullptr;
        }
        return func;
    }

}