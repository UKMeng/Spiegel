#include <Spiegel.h>

class ExampleLayer : public spg::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override {
		SPG_INFO("ExampleLayer::OnUpdate");
	}

	void OnEvent(spg::Event& event) override {
		SPG_INFO("{0}", event.ToString());
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