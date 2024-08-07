#include "spgpch.h"
#include "Renderer2D.h"

#include "Spiegel/Asset/AssetManager.h"
#include "Spiegel/Asset/Shader.h"
#include "Font.h"
#include "VertexArray.h"
#include "RenderCommand.h"
#include "UniformBuffer.h"
#include "Spiegel/Utils/PlatformUtils.h"

#include <glm/gtc/matrix_transform.hpp>



namespace spg {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		// Editor Only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;

		// Editor Only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor Only
		int EntityID;
	};

	struct TextVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;

		// Editor Only
		int EntityID;
	};

	// Temporary
	struct MaterialVertex
	{
		glm::vec3 Position;

		// Editor Only
		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture
		Ref<Texture2D> WhiteTexture;

		// -------------Quad-----------------
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		
		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;
		glm::vec4 QuadVertexPositions[4];
		// -------------Quad-----------------

		// -------------Circle---------------
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;
		// -------------Circle---------------

		// -------------Line-----------------
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;
		float LineWidth = 2.0f;
		// -------------Line-----------------

		// -------------Text-----------------
		Ref<VertexArray> TextVertexArray;
		Ref<VertexBuffer> TextVertexBuffer;
		Ref<Shader> TextShader;

		uint32_t TextIndexCount = 0;
		TextVertex* TextVertexBufferBase = nullptr;
		TextVertex* TextVertexBufferPtr = nullptr;
		// -------------Text-----------------

		// -------------Material-------------
		Ref<VertexArray> MaterialVertexArray;
		Ref<VertexBuffer> MaterialVertexBuffer;
		Ref<Shader> MaterialShader;

		uint32_t MaterialIndexCount = 0;
		MaterialVertex* MaterialVertexBufferBase = nullptr;
		MaterialVertex* MaterialVertexBufferPtr = nullptr;
		// -------------Material-------------

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData s_Renderer2DData;

	void Renderer2D::Init()
	{
		// -------------Quad-----------------
		s_Renderer2DData.QuadVertexArray = VertexArray::Create();
		s_Renderer2DData.QuadVertexBuffer = VertexBuffer::Create(s_Renderer2DData.MaxVertices * sizeof(QuadVertex));
		s_Renderer2DData.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" },
			{ ShaderDataType::Int, "a_EntityID" }
		});
		s_Renderer2DData.QuadVertexArray->AddVertexBuffer(s_Renderer2DData.QuadVertexBuffer);

		s_Renderer2DData.QuadVertexBufferBase = new QuadVertex[s_Renderer2DData.MaxVertices];

		// dangerous, TODO: fix this
		uint32_t* quadIndices = new uint32_t[s_Renderer2DData.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Renderer2DData.MaxIndices; i += 6) {
			// 0 1 2 2 3 0
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Renderer2DData.MaxIndices);
		s_Renderer2DData.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_Renderer2DData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Renderer2DData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_Renderer2DData.TextureSlots[0] = s_Renderer2DData.WhiteTexture;

		s_Renderer2DData.QuadShader = Shader::Create("assets/shaders/Quad.glsl");
		s_Renderer2DData.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Renderer2DData.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Renderer2DData.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Renderer2DData.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
		// -------------Quad-----------------

		// -------------Circle---------------
		s_Renderer2DData.CircleVertexArray = VertexArray::Create();
		s_Renderer2DData.CircleVertexBuffer = VertexBuffer::Create(s_Renderer2DData.MaxVertices * sizeof(CircleVertex));
		s_Renderer2DData.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float, "a_Thickness" },
			{ ShaderDataType::Int, "a_EntityID" }
			});
		s_Renderer2DData.CircleVertexArray->AddVertexBuffer(s_Renderer2DData.CircleVertexBuffer);
		s_Renderer2DData.CircleVertexArray->SetIndexBuffer(quadIB);
		s_Renderer2DData.CircleVertexBufferBase = new CircleVertex[s_Renderer2DData.MaxVertices];
		s_Renderer2DData.CircleShader = Shader::Create("assets/shaders/Circle.glsl");
		// -------------Circle---------------

		// -------------Line-----------------
		s_Renderer2DData.LineVertexArray = VertexArray::Create();
		s_Renderer2DData.LineVertexBuffer = VertexBuffer::Create(s_Renderer2DData.MaxVertices * sizeof(LineVertex));
		s_Renderer2DData.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Int, "a_EntityID" }
			});
		s_Renderer2DData.LineVertexArray->AddVertexBuffer(s_Renderer2DData.LineVertexBuffer);
		s_Renderer2DData.LineVertexBufferBase = new LineVertex[s_Renderer2DData.MaxVertices];
		s_Renderer2DData.LineShader = Shader::Create("assets/shaders/Line.glsl");
		// -------------Line-----------------

		// -------------Text-----------------
		s_Renderer2DData.TextVertexArray = VertexArray::Create();
		s_Renderer2DData.TextVertexBuffer = VertexBuffer::Create(s_Renderer2DData.MaxVertices * sizeof(TextVertex));
		s_Renderer2DData.TextVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Int, "a_EntityID" }
			});
		s_Renderer2DData.TextVertexArray->AddVertexBuffer(s_Renderer2DData.TextVertexBuffer);
		s_Renderer2DData.TextVertexArray->SetIndexBuffer(quadIB);
		s_Renderer2DData.TextVertexBufferBase = new TextVertex[s_Renderer2DData.MaxVertices];
		s_Renderer2DData.TextShader = AssetManager::GetShader("Text");
		// -------------Text-----------------

		// -------------Material-------------
		s_Renderer2DData.MaterialVertexArray = VertexArray::Create();
		s_Renderer2DData.MaterialVertexBuffer = VertexBuffer::Create(100 * sizeof(MaterialVertex));
		s_Renderer2DData.MaterialVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Int, "a_EntityID" }
			});
		s_Renderer2DData.MaterialVertexArray->AddVertexBuffer(s_Renderer2DData.MaterialVertexBuffer);
		s_Renderer2DData.MaterialVertexArray->SetIndexBuffer(quadIB);
		s_Renderer2DData.MaterialVertexBufferBase = new MaterialVertex[100];
		// -------------Material-------------

		s_Renderer2DData.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_Renderer2DData.QuadVertexBufferBase;
		delete[] s_Renderer2DData.CircleVertexBufferBase;
		delete[] s_Renderer2DData.LineVertexBufferBase;
		delete[] s_Renderer2DData.TextVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_Renderer2DData.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Renderer2DData.CameraUniformBuffer->SetData(&s_Renderer2DData.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		s_Renderer2DData.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Renderer2DData.CameraUniformBuffer->SetData(&s_Renderer2DData.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}


	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::Flush()
	{
		for (uint32_t i = 0; i < s_Renderer2DData.TextureSlotIndex; i++) {
			s_Renderer2DData.TextureSlots[i]->Bind(i);
		}

		if (s_Renderer2DData.QuadIndexCount) {
			uint32_t dataSize = (uint8_t*)s_Renderer2DData.QuadVertexBufferPtr - (uint8_t*)s_Renderer2DData.QuadVertexBufferBase;
			s_Renderer2DData.QuadVertexBuffer->SetData(s_Renderer2DData.QuadVertexBufferBase, dataSize);

			s_Renderer2DData.QuadShader->Bind();
			RenderCommand::DrawIndexed(s_Renderer2DData.QuadVertexArray, s_Renderer2DData.QuadIndexCount);
			s_Renderer2DData.Stats.DrawCalls++;
		}

		if (s_Renderer2DData.CircleIndexCount) {
			uint32_t dataSize = (uint8_t*)s_Renderer2DData.CircleVertexBufferPtr - (uint8_t*)s_Renderer2DData.CircleVertexBufferBase;
			s_Renderer2DData.CircleVertexBuffer->SetData(s_Renderer2DData.CircleVertexBufferBase, dataSize);

			s_Renderer2DData.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_Renderer2DData.CircleVertexArray, s_Renderer2DData.CircleIndexCount);
			s_Renderer2DData.Stats.DrawCalls++;
		}

		if (s_Renderer2DData.LineVertexCount) {
			uint32_t dataSize = (uint8_t*)s_Renderer2DData.LineVertexBufferPtr - (uint8_t*)s_Renderer2DData.LineVertexBufferBase;
			s_Renderer2DData.LineVertexBuffer->SetData(s_Renderer2DData.LineVertexBufferBase, dataSize);

			s_Renderer2DData.LineShader->Bind();
			RenderCommand::SetLineWidth(s_Renderer2DData.LineWidth);
			RenderCommand::DrawLines(s_Renderer2DData.LineVertexArray, s_Renderer2DData.LineVertexCount);
			s_Renderer2DData.Stats.DrawCalls++;
		}

		if (s_Renderer2DData.TextIndexCount) {
			uint32_t dataSize = (uint8_t*)s_Renderer2DData.TextVertexBufferPtr - (uint8_t*)s_Renderer2DData.TextVertexBufferBase;
			s_Renderer2DData.TextVertexBuffer->SetData(s_Renderer2DData.TextVertexBufferBase, dataSize);

			// TODO: For Text Renderer, Texture slot 32 is too small, should refactor
			s_Renderer2DData.TextShader->Bind();
			RenderCommand::DrawIndexed(s_Renderer2DData.TextVertexArray, s_Renderer2DData.TextIndexCount);
			s_Renderer2DData.Stats.DrawCalls++;
		}
	}

	void Renderer2D::StartBatch()
	{
		s_Renderer2DData.QuadIndexCount = 0;
		s_Renderer2DData.QuadVertexBufferPtr = s_Renderer2DData.QuadVertexBufferBase;

		s_Renderer2DData.CircleIndexCount = 0;
		s_Renderer2DData.CircleVertexBufferPtr = s_Renderer2DData.CircleVertexBufferBase;

		s_Renderer2DData.LineVertexCount = 0;
		s_Renderer2DData.LineVertexBufferPtr = s_Renderer2DData.LineVertexBufferBase;

		s_Renderer2DData.TextIndexCount = 0;
		s_Renderer2DData.TextVertexBufferPtr = s_Renderer2DData.TextVertexBufferBase;

		s_Renderer2DData.TextureSlotIndex = 1;
		
		// Temporary
		s_Renderer2DData.MaterialIndexCount = 0;
		s_Renderer2DData.MaterialVertexBufferPtr = s_Renderer2DData.MaterialVertexBufferBase;
	}
	
	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		constexpr size_t quadVertexCount = 4;
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_Renderer2DData.QuadIndexCount >= Renderer2DData::MaxIndices) {
			NextBatch();
		}

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Renderer2DData.TextureSlotIndex; i++) {
			if (*s_Renderer2DData.TextureSlots[i] == *texture) {
				// texture is already submitted
				textureIndex = (float)i;
				break;
			}
		}

		// texture is not submitted yet, submit it
		if (textureIndex == 0.0f) {
			textureIndex = (float)s_Renderer2DData.TextureSlotIndex;
			s_Renderer2DData.TextureSlots[s_Renderer2DData.TextureSlotIndex] = texture;
			s_Renderer2DData.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_Renderer2DData.QuadVertexBufferPtr->Position = transform * s_Renderer2DData.QuadVertexPositions[i];
			s_Renderer2DData.QuadVertexBufferPtr->Color = tintColor;
			s_Renderer2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Renderer2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Renderer2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Renderer2DData.QuadVertexBufferPtr++;
		}

		s_Renderer2DData.QuadIndexCount += 6;

		s_Renderer2DData.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Renderer2DData.QuadIndexCount >= Renderer2DData::MaxIndices) {
			NextBatch();
		}

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_Renderer2DData.QuadVertexBufferPtr->Position = transform * s_Renderer2DData.QuadVertexPositions[i];
			s_Renderer2DData.QuadVertexBufferPtr->Color = color;
			s_Renderer2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Renderer2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Renderer2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Renderer2DData.QuadVertexBufferPtr->EntityID = entityID;
			s_Renderer2DData.QuadVertexBufferPtr++;
		}

		s_Renderer2DData.QuadIndexCount += 6;

		s_Renderer2DData.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Renderer2DData.QuadIndexCount >= Renderer2DData::MaxIndices) {
			NextBatch();
		}

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Renderer2DData.TextureSlotIndex; i++) {
			if (*s_Renderer2DData.TextureSlots[i] == *texture) {
				// texture is already submitted
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)s_Renderer2DData.TextureSlotIndex;
			s_Renderer2DData.TextureSlots[s_Renderer2DData.TextureSlotIndex] = texture;
			s_Renderer2DData.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_Renderer2DData.QuadVertexBufferPtr->Position = transform * s_Renderer2DData.QuadVertexPositions[i];
			s_Renderer2DData.QuadVertexBufferPtr->Color = tintColor;
			s_Renderer2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Renderer2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Renderer2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Renderer2DData.QuadVertexBufferPtr->EntityID = entityID;
			s_Renderer2DData.QuadVertexBufferPtr++;
		}

		s_Renderer2DData.QuadIndexCount += 6;

		s_Renderer2DData.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Renderer2DData.QuadIndexCount >= Renderer2DData::MaxIndices) {
			NextBatch();
		}

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_Renderer2DData.QuadVertexBufferPtr->Position = transform * s_Renderer2DData.QuadVertexPositions[i];
			s_Renderer2DData.QuadVertexBufferPtr->Color = color;
			s_Renderer2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Renderer2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Renderer2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Renderer2DData.QuadVertexBufferPtr++;
		}

		s_Renderer2DData.QuadIndexCount += 6;

		s_Renderer2DData.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	/**
	 * @brief 
	 * @param position is center position
	 * @param size 
	 * @param rotation is in radians, if not, use glm::radians()
	 * @param texture 
	 * @param tilingFactor 
	 * @param tintColor 
	 */
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Renderer2DData.QuadIndexCount >= Renderer2DData::MaxIndices) {
			NextBatch();
		}

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Renderer2DData.TextureSlotIndex; i++) {
			if (*s_Renderer2DData.TextureSlots[i] == *texture) {
				// texture is already submitted
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)s_Renderer2DData.TextureSlotIndex;
			s_Renderer2DData.TextureSlots[s_Renderer2DData.TextureSlotIndex] = texture;
			s_Renderer2DData.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
			* glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_Renderer2DData.QuadVertexBufferPtr->Position = transform * s_Renderer2DData.QuadVertexPositions[i];
			s_Renderer2DData.QuadVertexBufferPtr->Color = tintColor;
			s_Renderer2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Renderer2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Renderer2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Renderer2DData.QuadVertexBufferPtr++;
		}

		s_Renderer2DData.QuadIndexCount += 6;

		s_Renderer2DData.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		constexpr size_t quadVertexCount = 4;
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_Renderer2DData.QuadIndexCount >= Renderer2DData::MaxIndices) {
			NextBatch();
		}

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Renderer2DData.TextureSlotIndex; i++) {
			if (*s_Renderer2DData.TextureSlots[i] == *texture) {
				// texture is already submitted
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)s_Renderer2DData.TextureSlotIndex;
			s_Renderer2DData.TextureSlots[s_Renderer2DData.TextureSlotIndex] = texture;
			s_Renderer2DData.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_Renderer2DData.QuadVertexBufferPtr->Position = transform * s_Renderer2DData.QuadVertexPositions[i];
			s_Renderer2DData.QuadVertexBufferPtr->Color = tintColor;
			s_Renderer2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Renderer2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Renderer2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Renderer2DData.QuadVertexBufferPtr++;
		}

		s_Renderer2DData.QuadIndexCount += 6;

		s_Renderer2DData.Stats.QuadCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture) {
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		}
		else {
			DrawQuad(transform, src.Color, entityID);
		}
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, int entityID)
	{
		constexpr size_t circleVertexCount = 4;

		// TODO: Optimize this, not just Circle Index Count
		if (s_Renderer2DData.CircleIndexCount >= Renderer2DData::MaxIndices) {
			NextBatch();
		}

		for (size_t i = 0; i < circleVertexCount; i++) {
			s_Renderer2DData.CircleVertexBufferPtr->WorldPosition = transform * s_Renderer2DData.QuadVertexPositions[i];
			s_Renderer2DData.CircleVertexBufferPtr->LocalPosition = s_Renderer2DData.QuadVertexPositions[i] * 2.0f;
			s_Renderer2DData.CircleVertexBufferPtr->Color = color;
			s_Renderer2DData.CircleVertexBufferPtr->Thickness = thickness;
			s_Renderer2DData.CircleVertexBufferPtr->EntityID = entityID;
			s_Renderer2DData.CircleVertexBufferPtr++;
		}

		s_Renderer2DData.CircleIndexCount += 6;

		s_Renderer2DData.Stats.CircleCount++;
	
	}

	void Renderer2D::DrawLine(const glm::vec3& startPoint, const glm::vec3& endPoint, const glm::vec4& color, int entityID)
	{
		// TODO: Optimize this, not just Line Vertex Count
		if (s_Renderer2DData.LineVertexCount >= Renderer2DData::MaxVertices) {
			NextBatch();
		}

		s_Renderer2DData.LineVertexBufferPtr->Position = startPoint;
		s_Renderer2DData.LineVertexBufferPtr->Color = color;
		s_Renderer2DData.LineVertexBufferPtr->EntityID = entityID;
		s_Renderer2DData.LineVertexBufferPtr++;

		s_Renderer2DData.LineVertexBufferPtr->Position = endPoint;
		s_Renderer2DData.LineVertexBufferPtr->Color = color;
		s_Renderer2DData.LineVertexBufferPtr->EntityID = entityID;
		s_Renderer2DData.LineVertexBufferPtr++;

		s_Renderer2DData.LineVertexCount += 2;

		s_Renderer2DData.Stats.LineCount++;
	
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color, entityID);
		DrawLine(p1, p2, color, entityID);
		DrawLine(p2, p3, color, entityID);
		DrawLine(p3, p0, color, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];

		for (size_t i = 0; i < 4; i++) {
			lineVertices[i] = transform * s_Renderer2DData.QuadVertexPositions[i];
		}

		DrawLine(lineVertices[0], lineVertices[1], color, entityID);
		DrawLine(lineVertices[1], lineVertices[2], color, entityID);
		DrawLine(lineVertices[2], lineVertices[3], color, entityID);
		DrawLine(lineVertices[3], lineVertices[0], color, entityID);
	}


	void Renderer2D::DrawText(const glm::mat4& transform, TextComponent& tc, int entityID)
	{
		constexpr size_t textVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f} };

		//const std::wstring wtext = PlatformUtils::string2Wstring(tc.Text);

		const std::wstring wtext = tc.Text;
		Ref<Font> font = tc.Font;
		float fontSize = (float)font->GetFontSize();
		
		// TODO: Optimize this
		if (s_Renderer2DData.TextIndexCount + wtext.length() * 6 >= Renderer2DData::MaxIndices) {
			NextBatch();
		}

		// Check character is loaded in the font
		font->CharacterLoader(wtext);

		glm::vec4 textVertexPositions[4];
		

		float x = 0.0f, y = 0.0f;

		std::wstring::const_iterator c;
		for (c = wtext.begin(); c != wtext.end(); c++) {
			const wchar_t ch = *c;

			if (ch == '\n') {
				x = 0.0f;
				y -= 1.0f;
				continue;
			}

			const Character& character = font->GetCharacter(ch);
			Ref<Texture2D> texture = character.Texture;

			float xpos = x + character.Bearing.x / fontSize;
			float ypos = y - (character.Size.y - character.Bearing.y) / fontSize;
			float w = character.Size.x / fontSize;
			float h = character.Size.y / fontSize;

			textVertexPositions[0] = { xpos,     ypos,	   0.0f, 1.0f };
			textVertexPositions[1] = { xpos + w, ypos,	   0.0f, 1.0f };
			textVertexPositions[2] = { xpos + w, ypos + h, 0.0f, 1.0f };
			textVertexPositions[3] = { xpos,	 ypos + h, 0.0f, 1.0f };

			float textureIndex = 0.0f;

			for (uint32_t i = 1; i < s_Renderer2DData.TextureSlotIndex; i++) {
				if (*s_Renderer2DData.TextureSlots[i] == *texture) {
					// texture is already submitted
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f) {
				textureIndex = (float)s_Renderer2DData.TextureSlotIndex;
				s_Renderer2DData.TextureSlots[s_Renderer2DData.TextureSlotIndex] = texture;
				s_Renderer2DData.TextureSlotIndex++;
			}

			for (size_t i = 0; i < textVertexCount; i++) {
				s_Renderer2DData.TextVertexBufferPtr->Position = transform * textVertexPositions[i];
				s_Renderer2DData.TextVertexBufferPtr->Color = tc.Color;
				s_Renderer2DData.TextVertexBufferPtr->TexCoord = textureCoords[i];
				s_Renderer2DData.TextVertexBufferPtr->TexIndex = textureIndex;
				s_Renderer2DData.TextVertexBufferPtr->EntityID = entityID;
				s_Renderer2DData.TextVertexBufferPtr++;
			}

			x += (character.Advance >> 6) / fontSize;

			s_Renderer2DData.TextIndexCount += 6;

			s_Renderer2DData.Stats.TextCount++;
		}
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Renderer2DData.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Renderer2DData.LineWidth = width;
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Renderer2DData.Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Renderer2DData.Stats, 0, sizeof(Statistics));
	}

	// Temporary
	void Renderer2D::DrawMaterial(const glm::mat4& transform, int entityID)
	{
		constexpr size_t materialVertexCount = 4;

		if (s_Renderer2DData.MaterialIndexCount >= 100) {
			NextBatch();
		}

		for (size_t i = 0; i < materialVertexCount; i++) {
			s_Renderer2DData.MaterialVertexBufferPtr->Position = transform * s_Renderer2DData.QuadVertexPositions[i];
			s_Renderer2DData.MaterialVertexBufferPtr->EntityID = entityID;
			s_Renderer2DData.MaterialVertexBufferPtr++;
		}

		s_Renderer2DData.MaterialIndexCount += 6;
	}

	void Renderer2D::SubmitMaterial(const Ref<Material>& material)
	{
		if (s_Renderer2DData.MaterialIndexCount) {
			uint32_t dataSize = (uint8_t*)s_Renderer2DData.MaterialVertexBufferPtr - (uint8_t*)s_Renderer2DData.MaterialVertexBufferBase;
			s_Renderer2DData.MaterialVertexBuffer->SetData(s_Renderer2DData.MaterialVertexBufferBase, dataSize);
			material->Upload();
			RenderCommand::DrawIndexed(s_Renderer2DData.MaterialVertexArray, s_Renderer2DData.MaterialIndexCount);
		}
		s_Renderer2DData.MaterialIndexCount = 0;
		s_Renderer2DData.MaterialVertexBufferPtr = s_Renderer2DData.MaterialVertexBufferBase;
	}

}