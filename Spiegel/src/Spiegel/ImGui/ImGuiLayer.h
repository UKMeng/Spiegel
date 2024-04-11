#pragma once

#include "Spiegel/Core/Layer.h"
#include "Spiegel/Events/ApplicationEvent.h"
#include "Spiegel/Events/KeyEvent.h"
#include "Spiegel/Events/MouseEvent.h"

namespace spg {
	class SPG_API ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();
		
		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}