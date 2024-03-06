#include "GraphScriptSandbox.h"
#include "imnodes.h"
#include "RuntimeUtils.h"
#include <filesystem>

gs::GraphScriptSandbox::GraphScriptSandbox(String projectDir, Context* context)
{
	p_Context = context;
	p_ProjectPath = projectDir;
	
	p_UserPath.resize(150);
	p_ProjectPath.resize(150);
	p_NewGraphName.resize(150);
	p_NewFunctionName.resize(150);
	p_NewDataSocketName.resize(150);
	p_NewVariableName.resize(150);
	p_FunctionToCallName.resize(150);

	Deserialize();
}

void gs::GraphScriptSandbox::OnImGui()
{
	int idCounter = 1;
	HashMap<void*, int> counterMap;
	HashMap<int, ExecutionSocket*> exeSocketMap;
	HashMap<int, DataSocket*> dataSocketMap;

	HandleEntryConfigs();

	Dockspace();

	if (ImGui::Begin("Root Menu"))
	{
		HandleMainMenu();

		HandleVariableSets();

		HandleInstances();
	}

	ImGui::End();

	for (int i = 0; i < p_Builders.size(); i++)
	{
		HandleGraphBuilderImGui(p_Builders[i], idCounter);
	}

	p_SetPositions = false;

}

void gs::GraphScriptSandbox::ParseGraphNodePositions(String& source, GraphBuilder* b)
{
	Vector<String> lines = utils::SplitStringByChar(source, '\n');

	bool foundNodePositions = false;
	for (int i = 0; i < lines.size(); i++)
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

void gs::GraphScriptSandbox::HandleMainMenu()
{
	ImGui::InputText("New Graph Name", p_NewGraphName.data(), 150);
	if (ImGui::Button("New Graph"))
	{
		utils::Trim(p_NewGraphName);
		p_Builders.push_back(p_Context->CreateBuilder(p_NewGraphName));
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

	if (ImGui::Button("Save Project"))
	{
		// TODO
		String data = Serialize();
		utils::SaveStringAtPath(data, p_ProjectPath);
	}

	ImGui::Separator();
}

void gs::GraphScriptSandbox::HandleEntryConfigs()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImVec2 menuSize = ImGui::GetItemRectSize();
		ImVec2 playTextWidth = ImGui::CalcTextSize("Play");
		ImVec2 configTextWidth = ImGui::CalcTextSize("Config");

		float centerX = (menuSize.x / 2.0f) - (playTextWidth.x / 2.0f) - (playTextWidth.x - 2.0f) - (configTextWidth.x / 2.0f);
		ImGui::SetCursorPos(ImVec2(centerX, ImGui::GetCursorPosY()));
		if (ImGui::Button("Play"))
		{
			if (p_SelectedGraph >= 0 && p_SelectedVariableSet >= 0 && p_SelectedFunctionName.m_Value != 0)
			{
				// Create .gse (graph script entry, much clever, very original)
				String entryConfig = SerializeSelectedEntryConfig();
				String entryConfigName = GetSelectedEntryConfigFileName();
				// place in project directory
				utils::SaveStringAtPath(entryConfig, entryConfigName);

				if (p_Process)
				{
					p_Process->Abort();
					delete p_Process;
					p_Process = nullptr;
				}
				// start & capture process of GS runtime, set working directory to project directory, argument the .gse path
				p_Process = Process::CreateAppProcess();
				SStream processCMD;
				processCMD << "GraphScript-Runtime.exe " << entryConfigName;
				p_Process->Run(".", processCMD.str());
			}
		}
		centerX += configTextWidth.x;
		ImGui::SetCursorPos(ImVec2(centerX, ImGui::GetCursorPosY()));
		if (ImGui::Button("Stop"))
		{
			if (p_Process)
			{
				p_Process->Abort();
				delete p_Process;
				p_Process = nullptr;
			}
		}

		centerX += configTextWidth.x;
		if (ImGui::BeginMenu("Config"))
		{
			if (ImGui::BeginMenu("Graph To Spawn"))
			{
				for (int i = 0; i < p_Builders.size(); i++)
				{
					if (ImGui::MenuItem(p_Builders[i]->m_Name.m_Original.c_str()))
					{
						p_SelectedGraph = i;
					}
					if (i == p_SelectedGraph)
					{
						ImGui::SameLine();
						ImGui::TextUnformatted("<---");
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Entry Function"))
			{
				if (p_SelectedGraph < 0)
				{
					ImGui::MenuItem("Please select a graph first");
				}
				else
				{
					for (auto& [name, f] : p_Builders[p_SelectedGraph]->m_Functions)
					{
						if (ImGui::MenuItem(name.m_Original.c_str()))
						{
							p_SelectedFunctionName = name;
						}

						if (p_SelectedFunctionName == name)
						{
							ImGui::SameLine();
							ImGui::TextUnformatted("<---");
						}
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Choose Variable Set"))
			{
				for (int i = 0; i < m_VariableSets.size(); i++)
				{
					String count = std::to_string(i);
					if (ImGui::MenuItem(count.c_str()))
					{
						p_SelectedVariableSet = i;
					}

					if (p_SelectedVariableSet == i)
					{
						ImGui::SameLine();
						ImGui::TextUnformatted("<---");
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void gs::GraphScriptSandbox::HandleVariableSets()
{
	if (ImGui::CollapsingHeader("Variable Sets"))
	{
		if (ImGui::Button("Add"))
		{
			m_VariableSets.push_back(RuntimeVariableSet());
		}
		for (int i = 0; i < m_VariableSets.size(); i++)
		{
			RuntimeVariableSet& set = m_VariableSets[i];
			SStream str;
			str << "Variable Set [" << i << "]";
			if (ImGui::CollapsingHeader(str.str().c_str()))
			{
				ImGui::Indent();
				ImGui::InputText("New Variable Name", p_NewVariableName.data(), 150);
				if (ImGui::BeginCombo("Variable Type", "Please Choose"))
				{
					for (auto& proto : p_Context->GetAllVariables())
					{
						if (ImGui::MenuItem(proto->m_Type.m_TypeHash.m_Original.c_str()))
						{
							utils::Trim(p_NewVariableName);
							set.emplace(p_NewVariableName, proto->Clone());
						}
					}
					ImGui::EndCombo();
				}
				ImGui::Separator();

				for (auto& [name, var] : set)
				{
					ImGui::Text(name.m_Original.c_str());
					HandleVariableInput(name, var);
					ImGui::Separator();
				}
			}
		}
	}

}

void gs::GraphScriptSandbox::HandleInstances()
{
	if (ImGui::CollapsingHeader("Instances"))
	{
		if (ImGui::BeginCombo("Add Instance", "Pick type"))
		{
			for (auto& builder : p_Builders)
			{
				if (ImGui::MenuItem(builder->m_Name.m_Original.c_str()))
				{
					p_Instances.push_back(p_Context->BuildGraph(builder));
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
		int indexToRemove = -1;
		if (ImGui::CollapsingHeader("Active Instances"))
		{
			ImGui::SliderInt("Variable Set", &p_SelectedVariableSet, -1, (int) m_VariableSets.size() - 1);
			for (int i = 0; i < p_Instances.size(); i++)
			{
				ImGui::Text("%d : %s", i, p_Instances[i]->m_Name.m_Original.c_str());
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					indexToRemove = i;
				}
				for (auto& [name, func] : p_Instances[i]->p_Functions)
				{
					if (ImGui::Button(name.m_Original.c_str()))
					{
						if (p_SelectedVariableSet > -1)
						{
							p_Instances[i]->CallFunction(name, ToVariableSet(m_VariableSets[p_SelectedVariableSet]));
						}
					}
				}
			}
		}
		if (indexToRemove > -1)
		{
			p_Context->DestroyGraph(p_Instances[indexToRemove]);
			p_Instances.erase(p_Instances.begin() + indexToRemove);
		}
	}

}

void gs::GraphScriptSandbox::HandleGraphBuilderImGui(GraphBuilder* builder, int& idCounter)
{
	HashMap<void*, int> counterMap;
	HashMap<int, ExecutionSocket*> exeSocketMap;
	HashMap<int, DataSocket*> dataSocketMap;
	HashMap<int, int> exeLinkCounter;
	HashMap<int, int> dataLinkCounter;

	SStream name;
	
	name << builder->m_Name.m_Original << " [Inspector]";

	if (ImGui::Begin(name.str().c_str()))
	{
		if(ImGui::CollapsingHeader("Functions"))
		{
			ImGui::InputText("New Function Name", p_NewFunctionName.data(), 150);
			ImGui::SameLine();
			if (ImGui::Button("Add Function") && !p_NewFunctionName.empty())
			{
				utils::Trim(p_NewFunctionName);
				builder->AddFunction(HashString(p_NewFunctionName));
			}
			ImGui::Indent();
			for (auto& [name, func] : builder->m_Functions)
			{
				if (ImGui::CollapsingHeader(name.m_Original.c_str()))
				{
					ImGui::Indent();
					ImGui::InputText("New Data Socket Name: ", p_NewDataSocketName.data(), 150);
					if (ImGui::BeginCombo("Add Output Socket (Parameter)", "Pick Type"))
					{
						for (auto& proto : p_Context->GetAllDataSockets())
						{
							if (ImGui::MenuItem(proto->m_Type.m_TypeHash.m_Original.c_str()) && !p_NewDataSocketName.empty())
							{
								utils::Trim(p_NewDataSocketName);
								func->m_OutputDataSockets.emplace(HashString(p_NewDataSocketName), proto->Clone());
								ResetString(p_NewDataSocketName);
							}
						}

						ImGui::EndCombo();
					}
					ImGui::Text("Parameters / Output Data Sockets");
					for (auto& [socketName, socket] : func->m_OutputDataSockets)
					{
						ImGui::Text("%s : %s", socketName.m_Original.c_str(), socket->m_Type.m_TypeHash.m_Original.c_str());
					}

					ImGui::Separator();
					ImGui::Text("Output Execution Sockets");
					for (auto&  socket : func->m_OutputExecutionSockets)
					{
						ImGui::Text("%s : Loop Count : %d", socket->m_SocketName.m_Original.c_str(), socket->m_LoopCount);
					}

					ImGui::Unindent();
				}
			}
			ImGui::Unindent();
		}
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Variables"))
		{
			ImGui::InputText("New Variable Name", p_NewVariableName.data(), 150);
			ImGui::SameLine();
			if(ImGui::BeginCombo("Add Output Socket (Parameter)", "Pick Type"))
			{
				for (auto& proto : p_Context->GetAllVariables())
				{
					if (ImGui::MenuItem(proto->m_Type.m_TypeHash.m_Original.c_str()) && !p_NewVariableName.empty())
					{
						utils::Trim(p_NewVariableName);
						builder->m_Variables.emplace(HashString(p_NewVariableName), proto->Clone());
						ResetString(p_NewVariableName);
					}
				}

				ImGui::EndCombo();
			}

			ImGui::Indent();
			HashString nameToDelete(-1);
			for (auto& [name, var] : builder->m_Variables)
			{
				ImGui::Text("%s : %s", name.m_Original.c_str(), var->m_Type.m_TypeHash.m_Original.c_str());
				ImGui::SameLine();
				ImGui::PushID((int)name.m_Value);
				if (ImGui::Button("Delete"))
				{
					nameToDelete = name;
				}
				HandleVariableInput(name, var.get());
				ImGui::PopID();
			}

			if (nameToDelete.m_Value != -1)
			{
				builder->DeleteVariable(nameToDelete);
			}
			ImGui::Unindent();
		}
		ImGui::Separator();
		if (ImGui::Button("Save to file"))
		{
			SaveGraph(builder);
		}
	}
	ImGui::End();

	ImGui::Begin(builder->m_Name.m_Original.c_str());

	ImNodes::BeginNodeEditor();

	HandleAddNodeMenu(builder, idCounter);

	HandleNodes(builder, idCounter, counterMap, exeSocketMap, dataSocketMap);

	idCounter = 65535;
	
	HandleVariableNodes(builder, idCounter, counterMap, dataSocketMap);
	
	HandleLinks(builder, idCounter, counterMap, exeLinkCounter, dataLinkCounter);

	ImNodes::EndNodeEditor();

	ImGui::End();

	HandleCreateDestroyLinks(builder, exeSocketMap, dataSocketMap, exeLinkCounter, dataLinkCounter);
}

void gs::GraphScriptSandbox::HandleAddNodeMenu(GraphBuilder* builder, int& idCounter)
{
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
	{
		p_ShowNodesPopup = true;
		p_PopupPos = ImGui::GetMousePos();
	}

	if (p_ShowNodesPopup)
	{

		ImGui::SetNextWindowPos(p_PopupPos);
		if (ImGui::Begin("Select Node", &p_ShowNodesPopup, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
		{
			ImGui::Spacing();
			for (Node* node : p_Context->GetAllNodes())
			{
				if (ImGui::MenuItem(node->m_NodeName.m_Original.c_str()))
				{
					p_ShowNodesPopup = false;
					builder->AddNode(node->Clone());
				}
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsWindowHovered())
			{
				p_ShowNodesPopup = false;
			}
			ImGui::Unindent();
		}
		ImGui::End();

	}
}

void gs::GraphScriptSandbox::HandleVariableNodes(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, DataSocket*>& dataSocketMap)
{
	for (auto& [name, var] : builder->m_Variables)
	{
		ImNodes::BeginNode(idCounter);
		int nodeId = idCounter;

		if (p_SetPositions)
		{
			if (p_NodePositions.find(builder) != p_NodePositions.end())
			{
				if (p_NodePositions[builder].find(nodeId) != p_NodePositions[builder].end())
				{
					vec2& pos = p_NodePositions[builder][nodeId];
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
		dataSocketMap[idCounter] = var->GetSocket();
		idCounter++;

		ImGui::Text(name.m_Original.c_str());
		ImNodes::EndOutputAttribute();
		ImNodes::PopColorStyle();

		ImNodes::EndNode();

		ImVec2 nodePos = ImNodes::GetNodeGridSpacePos(nodeId);
		p_NodePositions[builder][nodeId] = vec2{ nodePos.x, nodePos.y };
	}
}

void gs::GraphScriptSandbox::HandleNodes(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, ExecutionSocket*>& exeSocketMap, HashMap<int, DataSocket*>& dataSocketMap)
{
	bool anyNodeSelected = false;

	for (int j = 0; j < builder->m_Nodes.size(); j++)
	{
		Node* node = builder->m_Nodes[j];
		int nodeId = idCounter;

		bool popColour = false;

		if (builder->m_Functions.find(node->m_NodeName) != builder->m_Functions.end())
		{
			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(230, 100, 120, 255));
			popColour = true;
		}
		ImNodes::BeginNode(nodeId);

		if (ImNodes::IsNodeSelected(nodeId))
		{
			p_SelectedNode = j;
			anyNodeSelected = true;
		}

		counterMap.emplace(node, nodeId);
		idCounter++;

		if (p_SetPositions)
		{
			if (p_NodePositions.find(builder) != p_NodePositions.end())
			{
				if (p_NodePositions[builder].find(nodeId) != p_NodePositions[builder].end())
				{
					vec2& pos = p_NodePositions[builder][nodeId];
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
			exeSocketMap[idCounter] = execution;
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
			dataSocketMap[idCounter] = socket;
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
			exeSocketMap[idCounter] = execution;
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
			dataSocketMap[idCounter] = socket;
			idCounter++;

			ImGui::Text(name.m_Original.c_str());
			ImNodes::EndOutputAttribute();
			ImNodes::PopColorStyle();
		}
		ImNodes::EndNode();
		if (popColour)
		{
			ImNodes::PopColorStyle();
		}
		ImVec2 nodePos = ImNodes::GetNodeGridSpacePos(nodeId);
		p_NodePositions[builder][nodeId] = vec2{ nodePos.x, nodePos.y };
	}

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)) && p_SelectedNode > -1)
	{
		builder->DeleteNode(p_SelectedNode);
		p_SelectedNode = INT_MIN;
	}

	if (!anyNodeSelected)
	{
		p_SelectedNode = INT_MIN;
	}
}

void gs::GraphScriptSandbox::HandleLinks(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, int>& exeLinkCounter, HashMap<int, int>& dataLinkCounter)
{
	for (int j = 0; j < builder->m_ExecutionConnections.size(); j++)
	{
		auto conn = builder->m_ExecutionConnections[j];
		int link = idCounter;
		exeLinkCounter[link] = j;
		ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(255, 255, 255, 255));
		ImNodes::Link(idCounter++, counterMap[conn.m_RHS], counterMap[conn.m_LHS]);
		if (ImNodes::IsLinkSelected(link))
		{
			p_SelectedLink = link;
		}
		ImNodes::PopColorStyle();

	}

	for (int j = 0; j < builder->m_DataConnections.size(); j++)
	{
		auto conn = builder->m_DataConnections[j];
		dataLinkCounter[idCounter] = j;
		int link = idCounter;
		u64 hash = conn->m_LHS->m_Type.m_TypeHash.m_Value;
		ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(hash << 4, hash << 2, hash < 8, 255));
		ImNodes::Link(idCounter++, counterMap[conn->m_RHS], counterMap[conn->m_LHS]);
		if (ImNodes::IsLinkSelected(link))
		{
			p_SelectedLink = link;
		}
		ImNodes::PopColorStyle();
	}
}

void gs::GraphScriptSandbox::HandleCreateDestroyLinks(GraphBuilder* builder, HashMap<int, ExecutionSocket*>& exeSocketMap, HashMap<int, DataSocket*>& dataSocketMap, HashMap<int, int>& exeLinkCounter, HashMap<int, int>& dataLinkCounter)
{
	int start_attr, end_attr;
	if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
	{
		if (exeSocketMap.find(start_attr) != exeSocketMap.end() && exeSocketMap.find(end_attr) != exeSocketMap.end())
		{
			ExecutionSocket* start = exeSocketMap[start_attr];
			ExecutionSocket* end = exeSocketMap[end_attr];

			builder->ConnectExecutionSocket(start, end);
		}

		if (dataSocketMap.find(start_attr) != dataSocketMap.end() && dataSocketMap.find(end_attr) != dataSocketMap.end())
		{
			DataSocket* start = dataSocketMap[start_attr];
			DataSocket* end = dataSocketMap[end_attr];
			builder->ConnectDataSocket(start, end);
		}
	}
	if (p_SelectedLink > -1 && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
	{
		if (exeLinkCounter.find(p_SelectedLink) != exeLinkCounter.end())
		{
			builder->DestroyExecutionConnection(exeLinkCounter[p_SelectedLink]);
			p_SelectedLink = INT_MIN;
		}

		if (dataLinkCounter.find(p_SelectedLink) != dataLinkCounter.end())
		{
			builder->DestroyDataConnection(dataLinkCounter[p_SelectedLink]);
			p_SelectedLink = INT_MIN;
		}
	}
}

void gs::GraphScriptSandbox::HandleVariableInput(HashString name, Variable* var)
{
	if (var->m_Type.m_TypeHash.m_Value == 47833) // string typehash
	{
		VariableT<String>* strVar = (VariableT<String>*) var;

		String str = strVar->Get();

		if (str.size() <= 0)
		{
			str = "Hello World from Graph Script! Don't use commas or else!";
		}
		ImGui::InputText(name.m_Original.c_str(), str.data(), str.capacity());

		strVar->Set(str);
	}
	else if (var->m_Type.m_TypeHash == HashString("unsigned long"))
	{
		VariableT<u32>* u32Var= (VariableT<u32>*) var;

		u32 val = u32Var->Get();

		ImGui::InputScalar(name.m_Original.c_str(), ImGuiDataType_U32, &val);

		u32Var->Set(val);
	}

	else if (var->m_Type.m_TypeHash == HashString("long"))
	{
		VariableT<i32>* i32Var = (VariableT<i32>*) var;

		i32 val = i32Var->Get();

		ImGui::InputScalar(name.m_Original.c_str(), ImGuiDataType_S32, &val);

		i32Var->Set(val);
	}

	else if (var->m_Type.m_TypeHash == HashString("float"))
	{
		VariableT<float>* floatVar = (VariableT<float>*) var;

		float val = floatVar->Get();

		ImGui::InputScalar(name.m_Original.c_str(), ImGuiDataType_Float, &val);

		floatVar->Set(val);
	}

	else if (var->m_Type.m_TypeHash == HashString("bool"))
	{
		VariableT<bool>* boolVar = (VariableT<bool>*) var;

		bool val = boolVar->Get();
		ImGui::Checkbox(name.m_Original.c_str(), &val);

		boolVar->Set(val);
	}

	else
	{
		ImGui::Text("%s : No ImGui", name.m_Original.c_str());
	}
}

void gs::GraphScriptSandbox::ResetString(String& str)
{
	str = "";
	str.reserve(150);
}

void gs::GraphScriptSandbox::SaveGraph(GraphBuilder* builder)
{
	String gsString = builder->Serialize();

	SStream stream;

	stream << gsString;
	stream << "BeginNodePositions\n";
	for (auto& [index, pos] : p_NodePositions[builder])
	{
		stream << index << ":" << pos.x << ":" << pos.y << "\n";
	}
	stream << "EndNodePositions\n";

	String finalString = stream.str();
	SStream outputFileName;
	utils::Trim(builder->m_Name.m_Original);
	outputFileName << builder->m_Name.m_Original << ".gs";
	utils::SaveStringAtPath(finalString, outputFileName.str());
}

gs::String gs::GraphScriptSandbox::GetSelectedEntryConfigFileName()
{
	SStream stream;
	stream << p_Builders[p_SelectedGraph]->m_Name.m_Original << "___" << p_SelectedFunctionName.m_Original << ".gse";
	return stream.str();
}

gs::String gs::GraphScriptSandbox::SerializeSelectedEntryConfig()
{
	SStream stream;

	stream << "BeginGraphFiles\n";
	for (int i = 0; i < p_Builders.size(); i++)
	{
		SStream outputNameStream;
		outputNameStream << p_Builders[i]->m_Name.m_Original << ".gs";
		String outputName = outputNameStream.str();
		SaveGraph(p_Builders[i]);
		stream << outputName << "\n";
	}

	stream << "EndGraphFiles\nBeginEntryGraph\n";

	stream << p_Builders[p_SelectedGraph]->m_Name.m_Original << "\n";	
	
	stream << "EndEntryGraph\nBeginEntryArgs\n";

	SStream variableSetOutput;
	for (auto& [name, val] : m_VariableSets[p_SelectedVariableSet])
	{
		variableSetOutput << name.m_Original << ":" << val->m_Type.m_TypeHash.m_Value << ":" << utils::AnyToString(val->m_Value) << ",";
	}
	stream << variableSetOutput.str() << "\n";
	stream << "EndEntryArgs\nBeginFunctionName\n";
	stream << p_SelectedFunctionName.m_Original;
	stream << "\nEndFunctionName";

	return stream.str();
}

gs::String gs::GraphScriptSandbox::Serialize()
{
	SStream stream;

	stream << "BeginGraphs\n";
	for (int i = 0; i < p_Builders.size(); i++)
	{
		SStream outputNameStream;
		outputNameStream << p_Builders[i]->m_Name.m_Original << ".gs";
		String outputName = outputNameStream.str();
		SaveGraph(p_Builders[i]);
		stream << outputName << "\n";
	}
	
	stream << "EndGraphs\nBeginVariableSets\n";
	for (int i = 0; i < m_VariableSets.size(); i++)
	{
		SStream output;
		output << i << ",";
		for (auto& [name, val] : m_VariableSets[i])
		{
			output << name.m_Original << ":" << val->m_Type.m_TypeHash.m_Value << ":" << utils::AnyToString(val->m_Value) << ",";
		}
		stream << output.str() << "\n";
	}
	stream << "EndVariableSets\nBeginGraphInstances\n";
	for (int i = 0; i < p_Instances.size(); i++)
	{
		stream << p_Instances[i]->m_Name.m_Original << "\n";
	}
	stream << "EndGraphInstances\n";

	return stream.str();
}

void gs::GraphScriptSandbox::Deserialize()
{
	p_SetPositions = true;
	if (p_ProjectPath.empty())
	{
		return;
	}
	
	String projectData = utils::LoadStringAtPath(p_ProjectPath);
	Vector<String> lines = utils::SplitStringByChar(projectData, '\n');
	DeserializeState s = DeserializeState::Invalid;

	for (auto& line : lines)
	{
		DeserializeState previous = s;
		HandleCurrentState(s, line);

		if (s == DeserializeState::Invalid)
		{
			continue;
		}

		if (previous == DeserializeState::Invalid)
		{
			continue;
		}

		switch (s)
		{
		case Graphs:
			ParseGraph(line);
			break;
		case VariableSets:
			ParseVariableSet(line);
			break;
		case GraphInstances:
			ParseGraphInstance(line);
			break;
		case Invalid:
			break;
		default:
			break;
		}

	}

	String path = std::filesystem::current_path().string();
	SStream finalPath;
	finalPath << path << "/imgui.ini";
	ImGui::LoadIniSettingsFromDisk(finalPath.str().c_str());

}

void gs::GraphScriptSandbox::ParseGraph(String line)
{
	String source = utils::LoadStringAtPath(line);
	GraphBuilder* builder = p_Context->DeserializeGraph(source);
	ParseGraphNodePositions(source, builder);
	p_Builders.push_back(builder);
}

void gs::GraphScriptSandbox::ParseVariableSet(String line)
{
	Vector<String> parts = utils::SplitStringByChar(line, ',');

	GS_ASSERT(parts.size() > 0, "Not enough parts to be a variable set");

	if (parts.size() == 1)
	{
		return;
	}

	RuntimeVariableSet vars;

	for (int i = 1; i < parts.size(); i++)
	{
		Vector<String> components = utils::SplitStringByChar(parts[i], ':');
		GS_ASSERT(components.size() == 3, "Mismatched number of parts to be a variable");
		HashString name = components[0];
		u64 typeHash = std::stoull(components[1]);
		Any val = utils::StringToAny(components[2], typeHash);

		for (auto& proto : p_Context->GetAllVariables())
		{
			if (proto->m_Type.m_TypeHash.m_Value == typeHash)
			{
				Variable* clone = proto->Clone();
				clone->SetValue(val);
				vars.emplace(name, clone);
			}
		}
	}

	m_VariableSets.push_back(vars);

}

void gs::GraphScriptSandbox::ParseGraphInstance(String line)
{
	HashString name(line);
	for (int i = 0; i < p_Builders.size(); i++)
	{
		if (p_Builders[i]->m_Name == name)
		{
			p_Instances.push_back(p_Context->BuildGraph(p_Builders[i]));
			return;
		}
	}

}

void gs::GraphScriptSandbox::HandleCurrentState(DeserializeState& s, String& l)
{
	if (l == "BeginGraphs")
	{
		s = DeserializeState::Graphs;
	}
	if (l == "EndGraphs")
	{
		s = DeserializeState::Invalid;
	}

	if (l == "BeginVariableSets")
	{
		s = DeserializeState::VariableSets;
	}
	if (l == "EndVariableSets")
	{
		s = DeserializeState::Invalid;
	}

	if (l == "BeginGraphInstances")
	{
		s = DeserializeState::GraphInstances;
	}
	if (l == "EndGraphInstances")
	{
		s = DeserializeState::Invalid;
	}

}

gs::VariableSet gs::GraphScriptSandbox::ToVariableSet(RuntimeVariableSet& editorSet)
{
	VariableSet set;

	for (auto& [name, var] : editorSet)
	{
		set.emplace(name, var->m_Value);
	}
	return set;
}

void gs::GraphScriptSandbox::Dockspace()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 winPos = viewport->Pos;
	ImVec2 winSize = viewport->Size;
	ImGui::SetNextWindowPos({ winPos.x, winPos.y + 24 });
	ImGui::SetNextWindowSize(winSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, winSize, dockspace_flags);
	ImGui::End();
}