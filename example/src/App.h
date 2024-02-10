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
		GLFWwindow* p_Window;
	};
}