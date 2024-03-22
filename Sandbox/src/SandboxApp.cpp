#include <Spiegel.h>

class ExampleLayer : public spg::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override {
		if (spg::Input::IsKeyPressed(SPG_KEY_TAB)) {
			SPG_INFO("Tab key is pressed");
		}
	}

	void OnEvent(spg::Event& event) override {
		if (event.GetEventType() == spg::EventType::KeyPressed) {
			spg::KeyPressedEvent& e = (spg::KeyPressedEvent&)event;
			SPG_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
};

class Sandbox : public spg::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};

spg::Application* spg::CreateApplication() {
	return new Sandbox();
}