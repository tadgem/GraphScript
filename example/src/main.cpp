#include "App.h"
#include <imgui.h>
#include "imnodes.h"

int main() {
	using namespace gs;
	ExampleApp app;

	app.Init();

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
