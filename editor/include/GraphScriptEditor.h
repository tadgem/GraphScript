#ifndef GRAPH_SCRIPT_EDITOR_H
#define GRAPH_SCRIPT_EDITOR_H
#include "GraphScript.h"
namespace gs
{
	class GraphScriptEditor
	{
	public:
		GraphScriptEditor(Context* context, GraphBuilder* builder);

		void OnImGui();

	protected:
		Context* p_Context;
		GraphBuilder* p_Builder;

		struct vec2
		{
			float x;
			float y;
		};

		HashMap<u32, vec2> p_NodePositions;
	};
}

#endif