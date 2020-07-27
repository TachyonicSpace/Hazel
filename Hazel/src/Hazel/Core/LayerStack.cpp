#include "hzpch.h"
#include "LayerStack.h"

Hazel::LayerStack::LayerStack()
{
}

Hazel::LayerStack::~LayerStack()
{
	for (Layer* l : m_Layers)
	{
		l->OnDetach();
		delete l;
	}
}

void Hazel::LayerStack::PushLayer(Layer* layer)
{
	m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex++, layer);
}

void Hazel::LayerStack::PushOverlay(Layer* overlay)
{
	m_Layers.emplace_back(overlay);
}

void Hazel::LayerStack::PopLayer(Layer* layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
	if (it != m_Layers.begin() + m_LayerInsertIndex)
	{
		layer->OnDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
}

void Hazel::LayerStack::PopOverlay(Layer* overlay)
{
	auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
	if (it != m_Layers.end())
	{
		overlay->OnDetach();
		m_Layers.erase(it);
	}
}
