#include <Spiegel.h>

class ExampleLayer : public spg::Layer {
public:
	ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f) {
		m_VertexArray.reset(spg::VertexArray::Create());

		float vertices[3 * 7] = {
			// position          // color
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f,	1.0f, 1.0f, 0.0f, 1.0f
		};

		// Renderer API Abstraction
		// vertex buffer
		std::shared_ptr<spg::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(spg::VertexBuffer::Create(vertices, sizeof(vertices)));

		spg::BufferLayout layout = {
			{ spg::ShaderDataType::Float3, "a_Position" },
			{ spg::ShaderDataType::Float4, "a_Color" },
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// index buffer
		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<spg::IndexBuffer> indexBuffer;
		indexBuffer.reset(spg::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(spg::VertexArray::Create());
		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		std::shared_ptr<spg::VertexBuffer> squareVB;
		squareVB.reset(spg::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ spg::ShaderDataType::Float3, "a_Position" },
			});
		m_SquareVA->AddVertexBuffer(squareVB);
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<spg::IndexBuffer> squareIB;
		squareIB.reset(spg::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		// shader source code
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main() {
				v_Position = a_Position;
				v_Color = a_Color;	
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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

		m_Shader.reset(new spg::Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVextexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main() {
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main() {
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader.reset(new spg::Shader(blueShaderVextexSrc, blueShaderFragmentSrc));
	}

	void OnUpdate() override {
		// every single update shoule move the camera
		if (spg::Input::IsKeyPressed(SPG_KEY_W)) {
			m_CameraPosition.y += m_CameraMoveSpeed;
		}
		else if (spg::Input::IsKeyPressed(SPG_KEY_S)) {
			m_CameraPosition.y -= m_CameraMoveSpeed;
		}

		if (spg::Input::IsKeyPressed(SPG_KEY_A)) {
			m_CameraPosition.x -= m_CameraMoveSpeed;
		}
		else if (spg::Input::IsKeyPressed(SPG_KEY_D)) {
			m_CameraPosition.x += m_CameraMoveSpeed;
		}

		if (spg::Input::IsKeyPressed(SPG_KEY_Q)) {
			m_CameraRotation += m_CameraRotationSpeed;
		}
		else if (spg::Input::IsKeyPressed(SPG_KEY_E)) {
			m_CameraRotation -= m_CameraRotationSpeed;
		}
		
		spg::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		spg::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		spg::Renderer::BeginScene(m_Camera);
		spg::Renderer::Submit(m_BlueShader, m_SquareVA);
		spg::Renderer::Submit(m_Shader, m_VertexArray);
		spg::Renderer::EndScene();

	}

	void OnEvent(spg::Event& event) override {

	}
private:
	std::shared_ptr<spg::Shader> m_Shader;
	std::shared_ptr<spg::VertexArray> m_VertexArray;

	std::shared_ptr<spg::Shader> m_BlueShader;
	std::shared_ptr<spg::VertexArray> m_SquareVA;

	spg::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 0.05f;
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 1.0f;
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