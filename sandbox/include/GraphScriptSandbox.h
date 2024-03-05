#ifndef GRAPH_SCRIPT_EDITOR_H
#define GRAPH_SCRIPT_EDITOR_H
#include "GraphScript.h"
#include "imgui.h"
#include "RuntimeUtils.h"
namespace gs
{
	class GraphScriptSandbox
	{
	public:
		GraphScriptSandbox(String projectDir, Context* context);

		void OnImGui();

	protected:

		enum DeserializeState
		{
			Invalid = 0,
			// BeginGraphs/EndGraphs
			Graphs,
			// BeginVariableSets/EndVariableSets
			VariableSets,
			// BeginGraphInstances/EndGraphInstances
			GraphInstances,
		};

		Context*				p_Context;
		Vector<GraphBuilder*>	p_Builders;
		Vector<Graph*>			p_Instances;

		struct vec2
		{
			float x;
			float y;
		};

		HashMap<GraphBuilder*, HashMap<u32, vec2>> p_NodePositions;

		void ParseGraphNodePositions(String& source, GraphBuilder* b);
		
		void HandleMainMenu();
		void HandleEntryConfigs();
		void HandleVariableSets();
		void HandleInstances();
		void HandleGraphBuilderImGui(GraphBuilder* builder, int& idCounter);
		void HandleAddNodeMenu(GraphBuilder* builder, int& idCounter);
		void HandleVariableNodes(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, DataSocket*>& dataSocketMap);
		void HandleNodes(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, ExecutionSocket*>& exeSocketMap, HashMap<int, DataSocket*>& dataSocketMap);
		void HandleLinks(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, int>& exeLinkCounter, HashMap<int, int>& dataLinkCounter);
		void HandleCreateDestroyLinks(GraphBuilder* builder, HashMap<int, ExecutionSocket*>& exeSocketMap, HashMap<int, DataSocket*>& dataSocketMap, HashMap<int, int>& exeLinkCounter, HashMap<int, int>& dataLinkCounter);
		
		void HandleVariableInput(HashString name, Variable* var);

		void ResetString(String& str);
		void SaveGraph(GraphBuilder* builder);

		String	Serialize();
		void	Deserialize();
		void	ParseGraph(String line);
		void	ParseVariableSet(String line);
		void	ParseGraphInstance(String line);

		void HandleCurrentState(DeserializeState& s, String& l);

		String p_UserPath;
		String p_ProjectPath;
		String p_NewGraphName;
		String p_NewFunctionName;
		String p_NewDataSocketName;
		String p_NewVariableName;
		String p_FunctionToCallName;

		Vector<RuntimeVariableSet> m_VariableSets;

		VariableSet ToVariableSet(RuntimeVariableSet& editorSet);

		bool p_SetPositions = false;
		bool p_ShowNodesPopup = false;
		ImVec2 p_PopupPos;

		int p_SelectedNode = INT_MIN;
		int p_SelectedLink = INT_MIN;
		int p_SelectedVariableSet = -1;
	};
}

#endif