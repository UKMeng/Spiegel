#include "spgpch.h"
#include "Pipeline.h"

namespace spg
{
	void Pipeline::Exec(Ref<Scene> scene)
	{
		for (auto& pass: m_Passes)
		{
			pass->Exec(scene);
		}
	}

}