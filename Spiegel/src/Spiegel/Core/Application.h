#pragma once

#include "Base.h"
#include "Window.h"
#include "LayerStack.h"
#include "Spiegel/Events/Event.h"
#include "Spiegel/Events/ApplicationEvent.h"
#include "Spiegel/Core/Timestep.h"
#include "Spiegel/ImGui/ImGuiLayer.h"

#include "Spiegel/Asset/Shader.h"
#include "Spiegel/Renderer/Buffer.h"
#include "Spiegel/Renderer/VertexArray.h"

namespace spg {
	class Application
	{
	public:
		Application(const std::string& name = "Spiegel App");
		virtual ~Application();

		void Run();
		void Close();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow() { return *m_Window; }
		static Application& Get() { return *s_Instance; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application* GetApp() { return s_Instance; }
		static void SetApp(Application* app) { s_Instance = app; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};
	
	// To be defined in CLIENT
	Application* CreateApplication();
}


