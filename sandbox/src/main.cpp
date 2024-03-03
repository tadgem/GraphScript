#include "App.h"
#include <imgui.h>
#include "imnodes.h"
#include "GraphScript.h"
#include "GraphScriptEditor.h"
#include "BuiltInNodes.h"
#include "Utils.h"
#include "imgui_filedialog.h"

using namespace gs;

int main() {
	ExampleApp app("Sandbox");
	Context context;
	GraphScriptEditor* editor = nullptr;

	bool m_OpenProjectFileDialog = true;
	ImFileDialogInfo m_FileDialogInfo;
	m_FileDialogInfo.type = ImGuiFileDialogType_OpenFile;
	m_FileDialogInfo.title = "Open Project File";
	m_FileDialogInfo.fileName = "sample.gsp";
	m_FileDialogInfo.directoryPath = std::filesystem::current_path();

	while (app.ShouldRun())
	{
		if (m_OpenProjectFileDialog)
		{
			if (ImGui::FileDialog(&m_OpenProjectFileDialog, &m_FileDialogInfo))
			{
				// Result path in: m_fileDialogInfo.resultPath
				std::filesystem::current_path(m_FileDialogInfo.directoryPath);
				String projectPath = m_FileDialogInfo.resultPath.string();
				editor = new GraphScriptEditor(projectPath, &context);
			}
		}

		if (editor)
		{
			editor->OnImGui();
		}

		app.PostFrame();
	}

	delete editor;
}
