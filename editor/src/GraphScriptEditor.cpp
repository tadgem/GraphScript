#include "GraphScriptEditor.h"
#include "imnodes.h"
#include "imgui.h"
gs::GraphScriptEditor::GraphScriptEditor(Context* context, GraphBuilder* graphBuilder)
{
	p_Context = context;
	p_Builder = graphBuilder;
}

void gs::GraphScriptEditor::OnImGui()
{
	int idCounter = 1;
	HashMap<void*, int> counterMap;

	ImNodes::BeginNodeEditor();
	for (auto& [name, var] : p_Builder->m_Variables)
	{
		ImNodes::BeginNode(idCounter);
		counterMap.emplace(var.get(), idCounter);
		idCounter++;

		ImNodes::BeginOutputAttribute(idCounter, ImNodesPinShape_QuadFilled);

		counterMap.emplace(var->GetSocket(), idCounter);
		idCounter++;

		ImGui::Text(name.m_Original.c_str());
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();
	}

	for (Node* node : p_Builder->m_Nodes)
	{
		ImNodes::BeginNode(idCounter);

		counterMap.emplace(node, idCounter);
		idCounter++;

		ImNodes::BeginNodeTitleBar();
		ImGui::Text(node->m_NodeName.m_Original.c_str());
		ImNodes::EndNodeTitleBar();
		for (ExecutionSocket* execution : node->m_InputExecutionSockets)
		{
			ImNodes::BeginInputAttribute(idCounter, ImNodesPinShape_Triangle);

			counterMap.emplace(execution, idCounter);
			idCounter++;

			ImGui::Text(execution->m_SocketName.m_Original.c_str());
			ImNodes::EndInputAttribute();
		}
		for (auto [name, socket] : node->m_InputDataSockets)
		{
			ImNodes::BeginInputAttribute(idCounter, ImNodesPinShape_QuadFilled);

			counterMap.emplace(socket, idCounter);
			idCounter++;

			ImGui::Text(name.m_Original.c_str());
			ImNodes::EndInputAttribute();
		}
		for (ExecutionSocket* execution : node->m_OutputExecutionSockets)
		{
			ImNodes::BeginOutputAttribute(idCounter, ImNodesPinShape_Triangle);
			counterMap.emplace(execution, idCounter);
			idCounter++;
			ImGui::Text(execution->m_SocketName.m_Original.c_str());
			ImNodes::EndOutputAttribute();
		}
		for (auto [name, socket] : node->m_OutputDataSockets)
		{
			ImNodes::BeginOutputAttribute(idCounter, ImNodesPinShape_QuadFilled);

			counterMap.emplace(socket, idCounter);
			idCounter++;

			ImGui::Text(name.m_Original.c_str());
			ImNodes::EndOutputAttribute();
		}
		ImNodes::EndNode();
	}

	for (auto& conn : p_Builder->m_ExecutionConnections)
	{
		ImNodes::Link(idCounter++, counterMap[conn.m_RHS], counterMap[conn.m_LHS]);
	}

	for (auto& conn : p_Builder->m_DataConnections)
	{
		ImNodes::Link(idCounter++, counterMap[conn->m_RHS], counterMap[conn->m_LHS]);
	}

	ImNodes::EndNodeEditor();
}
