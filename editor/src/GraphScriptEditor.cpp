#include "GraphScriptEditor.h"
#include "imnodes.h"
#include "imgui.h"
#include "Utils.h"

gs::GraphScriptEditor::GraphScriptEditor(Context* context, GraphBuilder* graphBuilder)
{
	p_Context = context;
	p_Builder = graphBuilder;
}

void gs::GraphScriptEditor::OnImGui()
{
	int idCounter = 1;
	HashMap<void*, int> counterMap;

	if (ImGui::Begin("Debug"))
	{
		ImGui::Text("Node Positions");
		ImGui::Indent();
		for (auto& [index, pos] : p_NodePositions)
		{
			ImGui::Text("%d : {%f, %f}", index, pos.x, pos.y);
		}
		ImGui::Unindent();
		ImGui::Separator();
		if (ImGui::Button("Save to file"))
		{
			String gsString = p_Builder->Serialize();

			SStream stream;

			stream << gsString;
			stream << "BeginNodePositions\n";
			for (auto& [index, pos] : p_NodePositions)
			{
				stream << index << ":" << pos.x << ":" << pos.y << "\n";
			}
			stream << "EndNodePositions\n";

			String finalString = stream.str();
			utils::SaveStringAtPath(finalString, "output.gs");
		}
	}
	ImGui::End();

	ImNodes::BeginNodeEditor();
	for (auto& [name, var] : p_Builder->m_Variables)
	{
		ImNodes::BeginNode(idCounter);
		counterMap.emplace(var.get(), idCounter);
		idCounter++;
		u64 hash = var->m_Type.m_TypeHash.m_Value;
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(hash << 4, hash << 2, hash < 8, 255));
		ImNodes::BeginOutputAttribute(idCounter, ImNodesPinShape_QuadFilled);

		counterMap.emplace(var->GetSocket(), idCounter);
		idCounter++;

		ImGui::Text(name.m_Original.c_str());
		ImNodes::EndOutputAttribute();
		ImNodes::PopColorStyle();

		ImNodes::EndNode();
	}

	for (u32 i = 0; i < p_Builder->m_Nodes.size(); i++)
	{
		Node* node = p_Builder->m_Nodes[i];
		int nodeId = idCounter;
		ImNodes::BeginNode(nodeId);

		counterMap.emplace(node, nodeId);
		idCounter++;

		ImNodes::BeginNodeTitleBar();
		ImGui::Text(node->m_NodeName.m_Original.c_str());
		ImNodes::EndNodeTitleBar();
		for (ExecutionSocket* execution : node->m_InputExecutionSockets)
		{
			ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(255, 255, 255, 255));
			ImNodes::BeginInputAttribute(idCounter, ImNodesPinShape_Triangle);

			counterMap.emplace(execution, idCounter);
			idCounter++;

			ImGui::Text(execution->m_SocketName.m_Original.c_str());
			ImNodes::EndInputAttribute();
			ImNodes::PopColorStyle();
		}
		for (auto [name, socket] : node->m_InputDataSockets)
		{
			u64 hash = socket->m_Type.m_TypeHash.m_Value;
			ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(hash << 4, hash << 2, hash < 8, 255));
			ImNodes::BeginInputAttribute(idCounter, ImNodesPinShape_QuadFilled);

			counterMap.emplace(socket, idCounter);
			idCounter++;

			ImGui::Text(name.m_Original.c_str());
			ImNodes::EndInputAttribute();
			ImNodes::PopColorStyle();
		}
		for (ExecutionSocket* execution : node->m_OutputExecutionSockets)
		{
			ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(255, 255, 255, 255));
			ImNodes::BeginOutputAttribute(idCounter, ImNodesPinShape_Triangle);
			counterMap.emplace(execution, idCounter);
			idCounter++;
			ImGui::Text(execution->m_SocketName.m_Original.c_str());
			ImNodes::EndOutputAttribute();
			ImNodes::PopColorStyle();
		}
		for (auto [name, socket] : node->m_OutputDataSockets)
		{
			u64 hash = socket->m_Type.m_TypeHash.m_Value;
			ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(hash << 4, hash << 2, hash < 8, 255));
			ImNodes::BeginOutputAttribute(idCounter, ImNodesPinShape_QuadFilled);

			counterMap.emplace(socket, idCounter);
			idCounter++;

			ImGui::Text(name.m_Original.c_str());
			ImNodes::EndOutputAttribute();
			ImNodes::PopColorStyle();
		}
		ImNodes::EndNode();

		ImVec2 nodePos = ImNodes::GetNodeGridSpacePos(nodeId);
		p_NodePositions[i] = vec2{ nodePos.x, nodePos.y };
	}

	for (auto& conn : p_Builder->m_ExecutionConnections)
	{
		ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(255, 255, 255, 255));
		ImNodes::Link(idCounter++, counterMap[conn.m_RHS], counterMap[conn.m_LHS]);
		ImNodes::PopColorStyle();
	}

	for (auto& conn : p_Builder->m_DataConnections)
	{
		u64 hash = conn->m_LHS->m_Type.m_TypeHash.m_Value;
		ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(hash << 4, hash << 2, hash < 8, 255));
		ImNodes::Link(idCounter++, counterMap[conn->m_RHS], counterMap[conn->m_LHS]);
		ImNodes::PopColorStyle();
	}


	ImNodes::EndNodeEditor();
}
