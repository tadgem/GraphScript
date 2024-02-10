#include "App.h"
#include <iostream>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glad/glad.h>

#include "../vendor/imgui_impl_glfw.h"
#include "../vendor/imgui_impl_opengl3.h"

#define GLSL_VERSION "#version 330"

bool gs::ExampleApp::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	p_Window = glfwCreateWindow(1440, 900, "GraphScript Example", nullptr, nullptr);
	if (p_Window == nullptr) {
		std::cout << "Could not create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(p_Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Could not initialize GLAD" << std::endl;
		return -1;
	}

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = "imgui.ini";
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplGlfw_InitForOpenGL(p_Window, true);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);
}

bool gs::ExampleApp::ShouldRun()
{
	bool shouldClose = glfwWindowShouldClose(p_Window);

	if (shouldClose)
	{
		return false;
	}

	glfwPollEvents();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	return true;
}

void gs::ExampleApp::PostFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(p_Window);
}

void gs::ExampleApp::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(p_Window);
	glfwTerminate();
}
