#ifndef GRAPH_SCRIPT_EDITOR_H
#define GRAPH_SCRIPT_EDITOR_H
#include "GraphScript.h"
namespace gs
{
	class GraphScriptEditor
	{
	public:
		GraphScriptEditor(GraphBuilder* builder);

		void OnImGui();

		GraphBuilder* m_Builder;
	};
}

#endif