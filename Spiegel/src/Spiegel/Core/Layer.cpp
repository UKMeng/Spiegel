#include "spgpch.h"
#include "Layer.h"

namespace spg {
	Layer::Layer(const std::string& debugName)
		: m_DebugName(debugName) {}

	Layer::~Layer() {}
}