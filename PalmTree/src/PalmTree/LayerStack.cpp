#include "ptpch.h"
#include "LayerStack.h"

#include "Logging/Log.h"

namespace PalmTree {
    LayerStack::LayerStack() {}

    LayerStack::~LayerStack() {
        for (const Layer* layer : m_Layers) {
            delete layer;
        }
    }

    void LayerStack::DeleteLayer(Layer* layer) {
        auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);

        // If this layer is overlay
        if (it >= m_Layers.end() - m_NumOverlays)
            m_NumOverlays--;

        m_Layers.erase(it);

        layer->OnDetach();

        delete layer;
    }
}
