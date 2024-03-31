#include <Spiegel.h>

// --Entry Point
#include "Spiegel/Core/EntryPoint.h"
// ----------

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Sandbox2D.h"

class ExampleLayer : public spg::Layer {
public:
	ExampleLayer() : Layer("Example"), m_CameraController(1280.0f / 720.0f, true) {
		m_VertexArray = spg::VertexArray::Create();

		float vertices[3 * 7] = {
			// position          // color
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f,	1.0f, 1.0f, 0.0f, 1.0f
		};

		// Renderer API Abstraction
		// vertex buffer
		spg::Ref<spg::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(spg::VertexBuffer::Create(vertices, sizeof(vertices)));

		spg::BufferLayout layout = {
			{ spg::ShaderDataType::Float3, "a_Position" },
			{ spg::ShaderDataType::Float4, "a_Color" },
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// index buffer
		uint32_t indices[3] = { 0, 1, 2 };
		spg::Ref<spg::IndexBuffer> indexBuffer;
		indexBuffer.reset(spg::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA = spg::VertexArray::Create();
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		spg::Ref<spg::VertexBuffer> squareVB;
		squareVB.reset(spg::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ spg::ShaderDataType::Float3, "a_Position" },
			{ spg::ShaderDataType::Float2, "a_TexCoord" },
			});
		m_SquareVA->AddVertexBuffer(squareVB);
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		spg::Ref<spg::IndexBuffer> squareIB;
		squareIB.reset(spg::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		// shader source code
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main() {
				v_Position = a_Position;
				v_Color = a_Color;	
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main() {
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader = spg::Shader::Create("Triangle", vertexSrc, fragmentSrc);

		std::string blueShaderVextexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main() {
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			uniform vec3 u_Color;

			void main() {
				color = vec4(u_Color, 1.0);
			}
		)";

		m_BlueShader = spg::Shader::Create("blue", blueShaderVextexSrc, blueShaderFragmentSrc);


		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
	
		m_Texture = spg::Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = spg::Texture2D::Create("assets/textures/ustc.png");

		std::dynamic_pointer_cast<spg::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<spg::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0); // slot 0
	}

	void OnUpdate(spg::Timestep timestep) override {
		// SPG_TRACE("Delta time: {0}s ({1}ms)", timestep.GetSeconds(), timestep.GetMilliseconds());

		m_CameraController.OnUpdate(timestep);

		if (spg::Input::IsKeyPressed(SPG_KEY_J)) {
			m_SquarePosition.x -= m_SquareMoveSpeed * timestep;
		}
		else if (spg::Input::IsKeyPressed(SPG_KEY_L)) {
			m_SquarePosition.x += m_SquareMoveSpeed * timestep;
		}

		if (spg::Input::IsKeyPressed(SPG_KEY_I)) {
			m_SquarePosition.y += m_SquareMoveSpeed * timestep;
		}
		else if (spg::Input::IsKeyPressed(SPG_KEY_K)) {
			m_SquarePosition.y -= m_SquareMoveSpeed * timestep;
		}
		
		spg::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		spg::RenderCommand::Clear();



		spg::Renderer::BeginScene(m_CameraController.GetCamera());

		
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));


		std::dynamic_pointer_cast<spg::OpenGLShader>(m_BlueShader)->Bind();
		std::dynamic_pointer_cast<spg::OpenGLShader>(m_BlueShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++) {
			for (int x = 0; x < 20; x++) {
				glm::vec3 pos(m_SquarePosition.x + x * 0.11f, m_SquarePosition.y + y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				spg::Renderer::Submit(m_BlueShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");
		
		m_Texture->Bind();
		spg::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_LogoTexture->Bind();
		spg::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// spg::Renderer::Submit(m_Shader, m_VertexArray);
		spg::Renderer::EndScene();

	}

	virtual void OnImGuiRender() override {
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(spg::Event& e) override {
		m_CameraController.OnEvent(e);
	}
private:
	spg::ShaderLibrary m_ShaderLibrary;
	spg::Ref<spg::Shader> m_Shader;
	spg::Ref<spg::VertexArray> m_VertexArray;

	spg::Ref<spg::Shader> m_BlueShader;
	spg::Ref<spg::VertexArray> m_SquareVA;
	spg::Ref<spg::Texture2D> m_Texture;
	spg::Ref<spg::Texture2D> m_LogoTexture;

	spg::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquarePosition = { 0.0f, 0.0f, 0.0f };
	float m_SquareMoveSpeed = 1.0f;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};

class Sandbox : public spg::Application {
public:
	Sandbox() {
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {

	}
};

spg::Application* spg::CreateApplication() {
	return new Sandbox();
}