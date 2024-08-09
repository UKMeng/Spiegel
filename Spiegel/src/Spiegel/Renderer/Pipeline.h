#pragma once

#include "RenderPass.h"
#include "Spiegel/Scene/Scene.h"


namespace spg
{
	class Pipeline
	{
	public:
		Pipeline() = default;

		void Exec(Ref<Scene> scene);

		void AddPass(Ref<RenderPass> pass)
		{
			m_Passes.emplace_back(pass);
		}
		
	private:
		std::vector<Ref<RenderPass>> m_Passes;
	};

}

