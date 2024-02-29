#include "App.h"
#include <imgui.h>
#include "imnodes.h"
#include "GraphScript.h"
#include "GraphScriptEditor.h"
#include "BuiltInNodes.h"
#include "Utils.h"

using namespace gs;

const int ITERATIONS = 2;
int main() {
	ExampleApp app("Editor");
	Context context;


	GraphScriptEditor editor(&context);

	while (app.ShouldRun())
	{
		if(ImGui::Begin("Hello"))
		{
			editor.OnImGui();
		}
		ImGui::End();

		app.PostFrame();
	}
}
