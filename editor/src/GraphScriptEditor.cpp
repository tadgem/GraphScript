#include "GraphScriptEditor.h"
#include "imnodes.h"
#include "imgui.h"
gs::GraphScriptEditor::GraphScriptEditor(GraphBuilder* builder)
{
	m_Builder = builder;
}

void gs::GraphScriptEditor::OnImGui()
{
	ImNodes::BeginNodeEditor();
	for (Node* node : m_Builder->m_Nodes)
	{
		ImNodes::BeginNode(node->m_NodeName.m_Value);
		ImNodes::BeginNodeTitleBar();
		ImGui::Text(node->m_NodeName.m_Original.c_str());
		ImNodes::EndNodeTitleBar();
		for (ExecutionSocket* execution : node->m_InputExecutionSockets)
		{
			ImNodes::BeginInputAttribute(execution->m_SocketName.m_Value, ImNodesPinShape_Triangle);
			ImGui::Text(execution->m_SocketName.m_Original.c_str());
			ImNodes::EndInputAttribute();
		}
		for (auto [name, socket] : node->m_InputDataSockets)
		{
			ImNodes::BeginInputAttribute(name.m_Value, ImNodesPinShape_QuadFilled);
			ImGui::Text(name.m_Original.c_str());
			ImNodes::EndInputAttribute();
		}
		for (ExecutionSocket* execution : node->m_OutputExecutionSockets)
		{
			ImNodes::BeginOutputAttribute(execution->m_SocketName.m_Value, ImNodesPinShape_Triangle);
			ImGui::Text(execution->m_SocketName.m_Original.c_str());
			ImNodes::EndOutputAttribute();
		}
		for (auto [name, socket] : node->m_OutputDataSockets)
		{
			ImNodes::BeginOutputAttribute(name.m_Value, ImNodesPinShape_QuadFilled);
			ImGui::Text(name.m_Original.c_str());
			ImNodes::EndOutputAttribute();
		}
		ImNodes::EndNode();
	}

	ImNodes::EndNodeEditor();
}
