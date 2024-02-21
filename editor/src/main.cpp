#include "App.h"
#include <imgui.h>
#include "imnodes.h"
#include "GraphScript.h"
#include "BuiltInNodes.h"

const int ITERATIONS = 2;
int main() {
	
	gs::ExampleApp app("Editor");

	const int hardcoded_node_id = 1;

	while (app.ShouldRun())
	{
		if(ImGui::Begin("Hello"))
		{
			ImNodes::BeginNodeEditor();

			ImNodes::BeginNode(hardcoded_node_id);
			ImGui::Dummy(ImVec2(80.0f, 45.0f));
			ImNodes::EndNode();

			ImNodes::EndNodeEditor();
		}
		ImGui::End();

		app.PostFrame();
	}
}
