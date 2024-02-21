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
		ImNodes::BeginNode((int)node);
		ImNodes::BeginNodeTitleBar();
		ImGui::Text(node->m_NodeName.m_Original.c_str());
		ImNodes::EndNodeTitleBar();
		for (ExecutionSocket* execution : node->m_InputExecutionSockets)
		{
			ImNodes::BeginInputAttribute((int)execution, ImNodesPinShape_Triangle);
			ImGui::Text(execution->m_SocketName.m_Original.c_str());
			ImNodes::EndInputAttribute();
		}
		for (auto [name, socket] : node->m_InputDataSockets)
		{
			ImNodes::BeginInputAttribute((int)socket, ImNodesPinShape_QuadFilled);
			ImGui::Text(name.m_Original.c_str());
			ImNodes::EndInputAttribute();
		}
		for (ExecutionSocket* execution : node->m_OutputExecutionSockets)
		{
			ImNodes::BeginOutputAttribute((int)execution, ImNodesPinShape_Triangle);
			ImGui::Text(execution->m_SocketName.m_Original.c_str());
			ImNodes::EndOutputAttribute();
		}
		for (auto [name, socket] : node->m_OutputDataSockets)
		{
			ImNodes::BeginOutputAttribute((int)socket, ImNodesPinShape_QuadFilled);
			ImGui::Text(name.m_Original.c_str());
			ImNodes::EndOutputAttribute();
		}
		ImNodes::EndNode();
	}
	/*
	for (auto dataConn : m_Builder->m_DataConnections)
	{
		ImNodes::Link((int)dataConn, (int)dataConn->m_LHS, (int)dataConn->m_RHS);
	}

	for (auto exeConn : m_Builder->m_ExecutionConnections)
	{
		ImNodes::Link((int)&exeConn, (int)exeConn.m_LHS, (int)exeConn.m_RHS);
	}
	*/
	ImNodes::EndNodeEditor();
}
