#pragma once

struct GLFWwindow;

namespace gs
{
	class ExampleApp
	{
	public:
		bool Init();

		bool ShouldRun();

		void PostFrame();

		void Shutdown();

	protected:

		void Dockspace();
		GLFWwindow* p_Window;
	};
}