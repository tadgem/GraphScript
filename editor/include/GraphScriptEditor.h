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
		
		void HandleDebugMenu();
		void HandleGraphBuilderImGui(GraphBuilder* builder, int& idCounter);
		void HandleAddNodeMenu(GraphBuilder* builder);
		void HandleVariableNodes(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, DataSocket*>& dataSocketMap);
		void HandleNodes(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, ExecutionSocket*>& exeSocketMap, HashMap<int, DataSocket*>& dataSocketMap);
		void HandleLinks(GraphBuilder* builder, int& idCounter, HashMap<void*, int>& counterMap, HashMap<int, int> exeLinkCounter, HashMap<int, int> dataLinkCounter);
		void HandleCreateDestroyLinks(GraphBuilder* builder, HashMap<int, ExecutionSocket*>& exeSocketMap, HashMap<int, DataSocket*>& dataSocketMap, HashMap<int, int> exeLinkCounter, HashMap<int, int> dataLinkCounter);

		String p_UserPath;

		bool p_SetPositions = false;
		bool p_ShowNodesPopup = false;
		ImVec2 p_PopupPos;

		int p_SelectedNode = INT_MIN;
		int p_SelectedLink = INT_MIN;
	};
}

#endif