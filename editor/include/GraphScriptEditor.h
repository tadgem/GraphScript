#ifndef GRAPH_SCRIPT_EDITOR_H
#define GRAPH_SCRIPT_EDITOR_H
#include "GraphScript.h"
#include "imgui.h"
namespace gs
{
	class GraphScriptEditor
	{
	public:
		GraphScriptEditor(Context* context);

		void OnImGui();

	protected:
		Context* p_Context;
		Vector<GraphBuilder*> p_Builders;

		struct vec2
		{
			float x;
			float y;
		};

		HashMap<GraphBuilder*, HashMap<u32, vec2>> p_NodePositions;

		void ParseGraphNodePositions(String& source, GraphBuilder* b);
		
		void HandleGraphBuilderImGui(GraphBuilder* builder, int& idCounter);
		void HandleAddNodeMenu(GraphBuilder* builder);
		void HandleVariableNodes(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, DataSocket*>& dataSocketMap);
		void HandleNodes(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, ExecutionSocket*>& exeSocketMap, HashMap<int, DataSocket*>& dataSocketMap);
		void HandleLinks(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, int>& exeLinkCounter, HashMap<int, int>& dataLinkCounter);
		void HandleCreateDestroyLinks(GraphBuilder* builder, HashMap<int, ExecutionSocket*>& exeSocketMap, HashMap<int, DataSocket*>& dataSocketMap, HashMap<int, int>& exeLinkCounter, HashMap<int, int>& dataLinkCounter);
		
		void ResetString(String& str);


		String p_UserPath;
		String p_ProjectPath;
		String p_NewGraphName;
		String p_NewFunctionName;
		String p_NewDataSocketName;
		String p_NewVariableName;
		
		using EditorVariableSet = HashMap<HashString, Variable*>;

		Vector<EditorVariableSet> m_VariableSets;

		bool p_SetPositions = false;
		bool p_ShowNodesPopup = false;
		ImVec2 p_PopupPos;

		int p_SelectedNode = INT_MIN;
		int p_SelectedLink = INT_MIN;
	};
}

#endif