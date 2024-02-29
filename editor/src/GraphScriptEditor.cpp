#include "GraphScriptEditor.h"
#include "imnodes.h"
#include "Utils.h"



gs::GraphScriptEditor::GraphScriptEditor(Context* context)
{
	p_Context = context;
	p_UserPath.resize(150);
}

void gs::GraphScriptEditor::OnImGui()
{
	int idCounter = 1;
	HashMap<void*, int> counterMap;

	if (ImGui::Begin("Menu"))
	{
		if (ImGui::Button("New Graph"))
		{
			p_Builders.push_back(p_Context->CreateBuilder());
		}

		ImGui::InputText("Path to load", p_UserPath.data(), 150);
		if (ImGui::Button("Load Graph"))
		{
			String source = utils::LoadStringAtPath(p_UserPath);
			if (!source.empty())
			{
				GraphBuilder* builder = p_Builders.emplace_back(p_Context->DeserializeGraph(source));
				ParseGraphNodePositions(source, builder);
			}
		}

	}
	ImGui::End();

	for (int i = 0; i < p_Builders.size(); i++)
	{
		if (ImGui::Begin("Debug"))
		{
			ImGui::Text("Node Positions");
			ImGui::Indent();
			for (auto& [index, pos] : p_NodePositions[p_Builders[i]])
			{
				ImGui::Text("%d : {%f, %f}", index, pos.x, pos.y);
			}
			ImGui::Unindent();
			ImGui::Separator();
			if (ImGui::Button("Save to file"))
			{
				String gsString = p_Builders[i]->Serialize();

				SStream stream;

				stream << gsString;
				stream << "BeginNodePositions\n";
				for (auto& [index, pos] : p_NodePositions[p_Builders[i]])
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

		if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			p_ShowNodesPopup = true;
			p_PopupPos = ImGui::GetMousePos();
		}

		if (p_ShowNodesPopup)
		{
			ImGui::SetCursorPos(p_PopupPos);
			if (ImGui::BeginChild("Help", ImVec2(400, 0), true, ImGuiWindowFlags_AlwaysAutoResize))
			{
				for (Node* node : p_Context->GetAllNodes())
				{
					if (ImGui::MenuItem(node->m_NodeName.m_Original.c_str()))
					{
						p_ShowNodesPopup = false;
						p_Builders[i]->AddNode(node->Clone());
					}
				}
				ImGui::Separator();
				if (ImGui::Button("Close"))
				{
					p_ShowNodesPopup = false;
				}
				ImGui::EndChild();
			}
		}
		for (auto& [name, var] : p_Builders[i]->m_Variables)
		{
			ImNodes::BeginNode(idCounter);
			int nodeId = idCounter;

			if (p_SetPositions)
			{
				if (p_NodePositions.find(p_Builders[i]) != p_NodePositions.end())
				{
					if (p_NodePositions[p_Builders[i]].find(nodeId) != p_NodePositions[p_Builders[i]].end())
					{
						vec2& pos = p_NodePositions[p_Builders[i]][nodeId];
						ImNodes::SetNodeGridSpacePos(nodeId, ImVec2{ pos.x, pos.y });
					}
				}
			}

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

			ImVec2 nodePos = ImNodes::GetNodeGridSpacePos(nodeId);
			p_NodePositions[p_Builders[i]][nodeId] = vec2{ nodePos.x, nodePos.y };
		}
		
		int indexToDelete = -1;
		for (int j = 0; j < p_Builders[i]->m_Nodes.size(); j++)
		{
			Node* node = p_Builders[i]->m_Nodes[j];
			int nodeId = idCounter;
			ImNodes::BeginNode(nodeId);

			if (ImNodes::IsNodeSelected(nodeId))
			{
				p_SelectedNodeId = nodeId;
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Delete) && p_SelectedNodeId == nodeId)
			{
				indexToDelete = j;
			}

			counterMap.emplace(node, nodeId);
			idCounter++;

			if (p_SetPositions)
			{
				if (p_NodePositions.find(p_Builders[i]) != p_NodePositions.end())
				{
					if (p_NodePositions[p_Builders[i]].find(nodeId) != p_NodePositions[p_Builders[i]].end())
					{
						vec2& pos = p_NodePositions[p_Builders[i]][nodeId];
						ImNodes::SetNodeGridSpacePos(nodeId, ImVec2{ pos.x, pos.y });
					}
				}
			}

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
			p_NodePositions[p_Builders[i]][nodeId] = vec2{ nodePos.x, nodePos.y };
		}

		for (auto& conn : p_Builders[i]->m_ExecutionConnections)
		{
			ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(255, 255, 255, 255));
			ImNodes::Link(idCounter++, counterMap[conn.m_RHS], counterMap[conn.m_LHS]);
			ImNodes::PopColorStyle();
		}

		for (auto& conn : p_Builders[i]->m_DataConnections)
		{
			u64 hash = conn->m_LHS->m_Type.m_TypeHash.m_Value;
			ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(hash << 4, hash << 2, hash < 8, 255));
			ImNodes::Link(idCounter++, counterMap[conn->m_RHS], counterMap[conn->m_LHS]);
			ImNodes::PopColorStyle();
		}


		ImNodes::EndNodeEditor();
	}

	p_SetPositions = false;

}

void gs::GraphScriptEditor::ParseGraphNodePositions(String& source, GraphBuilder* b)
{
	Vector<String> lines = utils::SplitStringByChar(source, '\n');

	bool foundNodePositions = false;
	for (int i = 0; i < lines.size() ;i ++)
	{
		String& line = lines[i];
		if (line == "EndNodePositions")
		{
			break;
		}
		if (line == "BeginNodePositions")
		{
			foundNodePositions = true;
			continue;
		}
		if (!foundNodePositions)
		{
			continue;
		}

		Vector<String> parts = utils::SplitStringByChar(line, ':');
		i32 nodeIndex = std::stoi(parts[0]);
		float x = std::stof(parts[1]);
		float y = std::stof(parts[2]);

		p_NodePositions[b][nodeIndex] = vec2{ x, y };
		p_SetPositions = true;

	}
}
