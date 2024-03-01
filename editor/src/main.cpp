#include "App.h"
#include <imgui.h>
#include "imnodes.h"
#include "GraphScript.h"
#include "GraphScriptEditor.h"
#include "BuiltInNodes.h"
#include "Utils.h"

using namespace gs;

int main() {
	ExampleApp app("Editor");
	Context context;
	GraphScriptEditor editor(&context);

	while (app.ShouldRun())
	{
		editor.OnImGui();
		app.PostFrame();
	}
}
