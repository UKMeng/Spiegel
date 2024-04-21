#pragma once

#include "Spiegel/Core/Application.h"
#include "Entity.h"

namespace spg {
	class ScriptableEntity
	{
	public:
		ScriptableEntity() = default;
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void SetApplicationInstance(Application* instance) {
			Application::SetApp(instance);
		}
	private:
		Entity m_Entity;
		friend class Scene;
	};
}