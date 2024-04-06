#include <Spiegel.h>
#include <Spiegel/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace spg {

	class SpiegelEditor : public Application {
	public:
		SpiegelEditor()
			: Application("Spiegel Editor")
		{
			PushLayer(new EditorLayer());
		}

		~SpiegelEditor() = default;
	};

	Application* CreateApplication()
	{
		return new SpiegelEditor();
	}
}
