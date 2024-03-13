#include <Spiegel.h>

class Sandbox : public spg::Application {
public:
	Sandbox() {

	}

	~Sandbox() {

	}
};

spg::Application* spg::CreateApplication() {
	return new Sandbox();
}