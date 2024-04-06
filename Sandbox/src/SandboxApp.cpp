#include <Spiegel.h>
#include "Spiegel/Core/EntryPoint.h"

#include "Sandbox2D.h"

class Sandbox : public spg::Application {
public:
	Sandbox() {
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {

	}
};

spg::Application* spg::CreateApplication() {
	return new Sandbox();
}