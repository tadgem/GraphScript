#pragma once
#include "GraphScript.h"

using namespace gs;

namespace gs {
	template<typename T>
	class MutliplyNodeT : public INode
	{
	public:
		MutliplyNodeT()
		{
			AddExecutionInput("in");
			AddExecutionOutput("out");
			AddDataInput<T>("input");
			AddDataInput<T>("multiple");
			AddDataOutput<T>("result");
		}

		void Process() override
		{
			IDataSocketDefT<T>* inputSocket = (IDataSocketDefT<T>*) m_InputDataSockets["input"];
			IDataSocketDefT<T>* multipleSocket = (IDataSocketDefT<T>*) m_InputDataSockets["multiple"];
			IDataSocketDefT<T>* resultSocket = (IDataSocketDefT<T>*) m_OutputDataSockets["result"];

			if (!inputSocket->Get().has_value() || !multipleSocket->Get().has_value())
			{
				return;
			}

			resultSocket->Set(inputSocket->Get().value() * multipleSocket->Get().value());
		}
	};

	template<typename T>
	class PrintNodeT : public INode
	{
	public:
		PrintNodeT()
		{
			AddExecutionInput("in");
			AddExecutionOutput("out");
			AddDataInput<T>("input");
		}

		void Process() override
		{
			IDataSocketDefT<T>* inputSocket = (IDataSocketDefT<T>*) m_InputDataSockets["input"];

			if (!inputSocket->Get().has_value())
			{
				return;
			}

			std::cout << inputSocket->Get().value() << std::endl;
		}
	};

	class IfNode : public INode
	{
	public:
		IfNode()
		{
			AddExecutionInput("in");
			AddExecutionOutput("true");
			AddExecutionOutput("false");
			AddDataInput<bool>("condition");
		}

		void Process() override
		{
			IDataSocketDefT<bool>* conditionSocket = (IDataSocketDefT<bool>*) m_InputDataSockets["condition"];
			gs::IExecutionSocket* ifTrueExecutionSocket = m_OutputExecutionSockets[0];
			gs::IExecutionSocket* ifFalseExecutionSocket = m_OutputExecutionSockets[1];

			if (!conditionSocket->Get().has_value())
			{
				return;
			}
			bool condition = conditionSocket->Get().value();

			if (condition)
			{
				ifTrueExecutionSocket->SetShouldExecute(true);
				ifFalseExecutionSocket->SetShouldExecute(false);
			}
			else
			{
				ifTrueExecutionSocket->SetShouldExecute(false);
				ifFalseExecutionSocket->SetShouldExecute(true);
			}
		}
	};

	class ForNode : public INode 
	{
	public:
		ForNode()
		{
			AddExecutionInput("in");
			AddExecutionOutput("out");
			AddExecutionOutput("iter");
			AddDataInput<u32>("count");
		}

		void Process() override
		{
			IDataSocketDefT<u32>* loopCountSocket = (IDataSocketDefT<u32>*) m_InputDataSockets["count"];
			gs::IExecutionSocket* outExecutionSocket = m_OutputExecutionSockets[0];
			gs::IExecutionSocket* iterExecutionSocket = m_OutputExecutionSockets[1];

			if (!loopCountSocket->Get().has_value())
			{
				return;
			}
			u32 loopCount = loopCountSocket->Get().value();
			u32 iterExecutionCount = iterExecutionSocket->m_LoopCount;

			if (iterExecutionCount >= loopCount)
			{
				iterExecutionSocket->SetShouldExecute(false);
				outExecutionSocket->SetShouldExecute(true);
				iterExecutionSocket->m_LoopCount = 0;
			}
			else
			{
				iterExecutionSocket->SetShouldExecute(true);
				outExecutionSocket->SetShouldExecute(false);
				iterExecutionCount++;
			}

		}
	};

}