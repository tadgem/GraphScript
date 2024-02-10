#include "App.h"
#include <imgui.h>

int main() {
	using namespace gs;
	ExampleApp app;

	app.Init();

	while (app.ShouldRun())
	{
		if(ImGui::Begin("Hello"))
		{

		}
		ImGui::End();

		app.PostFrame();
	}

}
