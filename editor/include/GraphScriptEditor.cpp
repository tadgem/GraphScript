#include "GraphScriptEditor.h"
#include "imnodes.h"
#include "imgui.h"
gs::GraphScriptEditor::GraphScriptEditor(GraphBuilder* builder)
{
	m_Builder = builder;
}

void gs::GraphScriptEditor::OnImGui()
{
	const int hardcoded_node_id = 1;
	ImNodes::BeginNodeEditor();

	ImNodes::BeginNode(hardcoded_node_id);
	ImGui::Dummy(ImVec2(80.0f, 45.0f));
	ImNodes::EndNode();

	ImNodes::EndNodeEditor();
}
