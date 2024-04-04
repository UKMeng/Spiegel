#include "spgpch.h"
#include "Application.h"
#include "Log.h"
#include "Input.h"
#include "KeyCodes.h"

#include "Spiegel/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace spg {

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		SPG_PROFILE_FUNCTION();

		SPG_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(SPG_BIND_EVENT_FN(Application::OnEvent));
		m_Window->SetVSync(false);

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {
		SPG_PROFILE_FUNCTION();

		// TODO
		// Renderer::Shutdown();
	}

	void Application::Run() {
		SPG_PROFILE_FUNCTION();

		while (m_Running) {
			SPG_PROFILE_SCOPE("Runloop");

			float time = (float)glfwGetTime(); // Platform::GetTime()
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized) {
				SPG_PROFILE_SCOPE("LayerStack OnUpdate");

				for (Layer* layer : m_LayerStack) {
					layer->OnUpdate(timestep);
				}
			}
			
			m_ImGuiLayer->Begin(); 
			{
				SPG_PROFILE_SCOPE("LayerStack OnImGuiRender");

				for (Layer* layer : m_LayerStack) {
					layer->OnImGuiRender();
				}
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::PushLayer(Layer* layer) {
		SPG_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* layer) {
		SPG_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		SPG_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(SPG_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(SPG_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled) break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		SPG_PROFILE_FUNCTION();

		// When the window is minimized, the width and height are set to 0
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}
