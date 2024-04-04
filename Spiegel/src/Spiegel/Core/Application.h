#pragma once

#include "Base.h"
#include "Window.h"
#include "LayerStack.h"
#include "Spiegel/Events/Event.h"
#include "Spiegel/Events/ApplicationEvent.h"
#include "Spiegel/Core/Timestep.h"
#include "Spiegel/ImGui/ImGuiLayer.h"

#include "Spiegel/Renderer/Shader.h"
#include "Spiegel/Renderer/Buffer.h"
#include "Spiegel/Renderer/VertexArray.h"
#include "Spiegel/Renderer/OrthographicCamera.h"

namespace spg {
	class SPG_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
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


