#include "spgpch.h"
#include "ScriptEngine.h"

namespace spg {

	struct ScriptEngineData
	{
        HMODULE hmod;
	};

	static ScriptEngineData* s_SEData;

	void ScriptEngine::Init()
	{
        s_SEData = new ScriptEngineData();

        s_SEData->hmod = LoadLibrary(L"assets/scripts/ExampleGame.dll");
        if (s_SEData->hmod == NULL)
        {
            DWORD dw = GetLastError();
            printf("Failed to load DLL: error code %d\n", dw);
            return;
        }
        return;
	}

	void ScriptEngine::Shutdown()
	{
        if (!FreeLibrary(s_SEData->hmod))
        {
            DWORD dw = GetLastError();
            printf("Failed to free DLL: error code %d\n", dw);
            return;
        }
        delete s_SEData;
	}

    ScriptFunc ScriptEngine::GetInstantiateScript(const char* funcName)
    {
        ScriptFunc func = (ScriptFunc)GetProcAddress(s_SEData->hmod, funcName);
        if (func == NULL)
        {
            DWORD dw = GetLastError();
            printf("Failed to get function: error code %d\n", dw);
            return nullptr;
        }
        return func;
    }

}