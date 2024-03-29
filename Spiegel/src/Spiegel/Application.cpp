#include "spgpch.h"
#include "Application.h"
#include "Log.h"
#include "Input.h"
#include "Renderer/Renderer.h"
#include "KeyCodes.h"

#include <GLFW/glfw3.h>

namespace spg {

	Application* Application::s_Instance = nullptr;

// 回调函数
// 修改成Lambdas函数格式，原因：
// https://cntransgroup.github.io/EffectiveModernCppChinese/6.LambdaExpressions/item34.html
// 
// std::placeholders::_1 是一个占位符
// std::bind，允许您将成员函数绑定到特定对象，并可以将参数传递给该函数。
//#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application() {
		SPG_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback([this](Event& e) {
			EventDispatcher dispatcher(e);
			dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) {
				m_Running = false;
				return true;
				});
			for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
				(*--it)->OnEvent(e);
				if (e.Handled) break;
			}
		});
		m_Window->SetVSync(false);

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {

	}

	void Application::Run() {
		while (m_Running) {
			float time = (float)glfwGetTime(); // Platform::GetTime()
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack) {
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* layer) {
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	// Item 34: Prefer lambdas to std::bind

	//void Application::OnEvent(Event& e) {
	//	EventDispatcher dispatcher(e);
	//	// Dispath(std::function<bool(T&)>)
	//	// dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
	//	dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) {
	//		m_Running = false;
	//		return true;
	//	});

	//	SPG_CORE_TRACE("{0}", e.ToString());
	//}

	//bool Application::OnWindowClose(WindowCloseEvent& e) {
	//	m_Running = false;
	//	return true;
	//}
}
