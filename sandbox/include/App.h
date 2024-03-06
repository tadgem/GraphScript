#pragma once
#include <chrono>
#include <ctime>
#include "TypeDef.h"
#include "imgui.h"
struct GLFWwindow;

namespace gs
{

	class Timer
	{
	public:
		void start()
		{
			m_StartTime = std::chrono::system_clock::now();
			m_bRunning = true;
		}

		void stop()
		{
			m_EndTime = std::chrono::system_clock::now();
			m_bRunning = false;
		}

		u64 elapsedMilliseconds()
		{
			std::chrono::time_point<std::chrono::system_clock> endTime;

			if (m_bRunning)
			{
				endTime = std::chrono::system_clock::now();
			}
			else
			{
				endTime = m_EndTime;
			}

			return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
		}

		u64 elapsedSeconds()
		{
			return elapsedMilliseconds() / 1000ll;
		}

	private:
		std::chrono::time_point<std::chrono::system_clock> m_StartTime;
		std::chrono::time_point<std::chrono::system_clock> m_EndTime;
		bool                                               m_bRunning = false;
	};

	class Process
	{
	public:
		virtual void Run(String workingDirectory, String& cmd) = 0;
		virtual void Abort() = 0;
		static Process* CreateAppProcess();
	};


#ifdef _WIN32
#include "windows.h"
	class WindowsProcess : public Process
	{
	public:
		void Run(String workingDirectory, String& cmd) override;
		void Abort() override;
	protected:
		PROCESS_INFORMATION p_ProcessInfo;
	};
#endif

	class ExampleApp
	{
	public:
		ExampleApp(const gs::String& name);

		bool Init();

		bool ShouldRun();

		void PostFrame();

		void Shutdown();

		ImVec2 GetUsableWindowSize();

	protected:
		const gs::String p_Name;
		GLFWwindow* p_Window;
	};
}

// Macros
#define TIMER(NAME, CODE_TO_TIME) \
gs::Timer _##NAME##_timer; \
_##NAME##_timer.start(); \
#CODE_TO_TIME ;\
_##NAME##_timer.stop(); \
std::cout << #NAME << " : " << _##NAME##_timer.elapsedMilliseconds() << "ms \n";
