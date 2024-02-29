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

		String p_UserPath;

		bool p_SetPositions = false;
		bool p_ShowNodesPopup = false;
		ImVec2 p_PopupPos;

		int p_SelectedNodeId = INT_MIN;
	};
}

#endif