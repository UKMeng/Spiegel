#pragma once

namespace spg {
	// forward declaration
	class ScriptableEntity;

	typedef ScriptableEntity* (*ScriptFunc)();

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();
		static ScriptFunc GetInstantiateScript(const char* funcName);
	};
}