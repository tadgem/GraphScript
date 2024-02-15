#pragma once
#include <chrono>
#include <ctime>
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

		double elapsedMilliseconds()
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

		double elapsedSeconds()
		{
			return elapsedMilliseconds() / 1000.0;
		}

	private:
		std::chrono::time_point<std::chrono::system_clock> m_StartTime;
		std::chrono::time_point<std::chrono::system_clock> m_EndTime;
		bool                                               m_bRunning = false;
	};

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

// Macros
#define TIMER(NAME, CODE_TO_TIME) \
gs::Timer _##NAME##_timer; \
_##NAME##_timer.start(); \
#CODE_TO_TIME ;\
_##NAME##_timer.stop(); \
std::cout << #NAME << " : " << _##NAME##_timer.elapsedMilliseconds() << "ms \n";
