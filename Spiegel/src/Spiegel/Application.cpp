#include "spgpch.h"

#include "Application.h"

#include "Log.h"

#include "Input.h"

#include <glad/glad.h>

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

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		// Vertex Array
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f,  0.0f,
			0.0f, 0.5f, 0.0f
		};

		// Renderer API Abstraction
		// vertex buffer
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		// tell OpenGL where layer it is
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		// index buffer
		uint32_t indices[3] = { 0, 1, 2 };
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		// shader source code
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main() {
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main() {
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
	}

	Application::~Application() {

	}

	void Application::Run() {
		while (m_Running) {
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();
			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate();
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
