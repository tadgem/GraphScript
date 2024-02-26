#pragma once
#include "GraphScript.h"

using namespace gs;

namespace gs {
	template<typename T>
	class MutliplyNodeT : public Node
	{
	public:
		MutliplyNodeT() : Node("Multiply")
		{
			AddExecutionInput("in");
			AddExecutionOutput("out");
			AddDataInput<T>("input");
			AddDataInput<T>("multiple");
			AddDataOutput<T>("result");
		}

		void Process() override
		{
			DataSocketT<T>* inputSocket		= (DataSocketT<T>*) m_InputDataSockets["input"];
			DataSocketT<T>* multipleSocket	= (DataSocketT<T>*) m_InputDataSockets["multiple"];
			DataSocketT<T>* resultSocket	= (DataSocketT<T>*) m_OutputDataSockets["result"];

			if (!inputSocket->Get().has_value() || !multipleSocket->Get().has_value())
			{
				return;
			}

			resultSocket->Set(inputSocket->Get().value() * multipleSocket->Get().value());
		}

		Node* Clone() override
		{
			return new MutliplyNodeT<T>();
		}
	};

	template<typename T>
	class PrintNodeT : public Node
	{
	public:
		PrintNodeT() : Node("Print")
		{
			AddExecutionInput("in");
			AddExecutionOutput("out");
			AddDataInput<T>("input");
		}

		void Process() override
		{
			DataSocketT<T>* inputSocket = (DataSocketT<T>*) m_InputDataSockets["input"];

			if (!inputSocket->Get().has_value())
			{
				return;
			}

			std::cout << inputSocket->Get().value() << std::endl;
		}

		Node* Clone() override
		{
			return new PrintNodeT<T>();
		}
	};

	class IfNode : public Node
	{
	public:
		IfNode() : Node("If")
		{
			AddExecutionInput("in");
			AddExecutionOutput("true");
			AddExecutionOutput("false");
			AddDataInput<bool>("condition");
		}

		void Process() override
		{
			DataSocketT<bool>* conditionSocket = (DataSocketT<bool>*) m_InputDataSockets["condition"];
			gs::ExecutionSocket* ifTrueExecutionSocket = m_OutputExecutionSockets[0];
			gs::ExecutionSocket* ifFalseExecutionSocket = m_OutputExecutionSockets[1];

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

		Node* Clone() override
		{
			return new IfNode();
		}
	};

	class ForNode : public Node 
	{
	public:
		ForNode() : Node("For")
		{
			AddExecutionInput("in");
			AddExecutionOutput("out");
			AddExecutionOutput("iter");
			AddDataInput<u32>("count");
		}

		void Process() override
		{
			DataSocketT<u32>* loopCountSocket = (DataSocketT<u32>*) m_InputDataSockets["count"];
			gs::ExecutionSocket* outExecutionSocket = m_OutputExecutionSockets[0];
			gs::ExecutionSocket* iterExecutionSocket = m_OutputExecutionSockets[1];

			// fails here, data connections are fucked.
			if (!loopCountSocket->Get().has_value())
			{
				return;
			}
			u32 loopCount = loopCountSocket->Get().value();
			u32 iterExecutionCount = iterExecutionSocket->m_LoopCount;

			if (iterExecutionCount <= 1)
			{
				iterExecutionSocket->SetShouldExecute(true);
				outExecutionSocket->SetShouldExecute(false);
				iterExecutionSocket->m_LoopCount = loopCount;
			}
			else
			{
				iterExecutionSocket->m_LoopCount--;

				if (iterExecutionSocket->m_LoopCount == 0)
				{
					iterExecutionSocket->SetShouldExecute(false);
					outExecutionSocket->SetShouldExecute(true);
				}
			}

		}

		Node* Clone() override
		{
			return new ForNode();
		}
	};

}