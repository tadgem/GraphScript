#pragma once
#include "GraphScript.h"

using namespace gs;

namespace gs {
	
	template<typename T>
	class AdditionNodeT : public Node
	{
	public:
		AdditionNodeT(HashString name) : Node(name)
		{
			AddExecutionInput("in");
			AddExecutionOutput("out");
			AddDataInput<T>("input");
			AddDataInput<T>("add");
			AddDataOutput<T>("result");
		}

		void Process() override
		{
			DataSocketT<T>* inputSocket = (DataSocketT<T>*) m_InputDataSockets["input"];
			DataSocketT<T>* addSocket = (DataSocketT<T>*) m_InputDataSockets["add"];
			DataSocketT<T>* resultSocket = (DataSocketT<T>*) m_OutputDataSockets["result"];

			if (!inputSocket->Get().has_value() || !addSocket->Get().has_value())
			{
				return;
			}

			resultSocket->Set(inputSocket->Get().value() + addSocket->Get().value());
		}

		Node* Clone() override
		{
			return new AdditionNodeT<T>(m_NodeName);
		}
	};

	template<typename T>
	class SubtractNodeT : public Node
	{
	public:
		SubtractNodeT(HashString name) : Node(name)
		{
			AddExecutionInput("in");
			AddExecutionOutput("out");
			AddDataInput<T>("input");
			AddDataInput<T>("add");
			AddDataOutput<T>("result");
		}

		void Process() override
		{
			DataSocketT<T>* inputSocket = (DataSocketT<T>*) m_InputDataSockets["input"];
			DataSocketT<T>* subSocket = (DataSocketT<T>*) m_InputDataSockets["add"];
			DataSocketT<T>* resultSocket = (DataSocketT<T>*) m_OutputDataSockets["result"];

			if (!inputSocket->Get().has_value() || !subSocket->Get().has_value())
			{
				return;
			}

			resultSocket->Set(inputSocket->Get().value() - subSocket->Get().value());
		}

		Node* Clone() override
		{
			return new SubtractNodeT<T>(m_NodeName);
		}
	};

	template<typename T>
	class MultiplyNodeT : public Node
	{
	public:
		MultiplyNodeT(HashString name) : Node(name)
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
			return new MultiplyNodeT<T>(m_NodeName);
		}
	};

	template<typename T>
	class DivideNodeT : public Node
	{
	public:
		DivideNodeT(HashString name) : Node(name)
		{
			AddExecutionInput("in");
			AddExecutionOutput("out");
			AddDataInput<T>("input");
			AddDataInput<T>("div");
			AddDataOutput<T>("result");
		}

		void Process() override
		{
			DataSocketT<T>* inputSocket = (DataSocketT<T>*) m_InputDataSockets["input"];
			DataSocketT<T>* divSocket = (DataSocketT<T>*) m_InputDataSockets["div"];
			DataSocketT<T>* resultSocket = (DataSocketT<T>*) m_OutputDataSockets["result"];

			if (!inputSocket->Get().has_value() || !divSocket->Get().has_value())
			{
				return;
			}

			resultSocket->Set(inputSocket->Get().value() / divSocket->Get().value());
		}

		Node* Clone() override
		{
			return new DivideNodeT<T>(m_NodeName);
		}
	};

	template<typename T>
	class PrintNodeT : public Node
	{
	public:
		PrintNodeT(HashString name) : Node(name)
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
			return new PrintNodeT<T>(m_NodeName);
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

	class CustomNode : public Node
	{
	public:
		using CustomNodeCallback = std::function<void(HashMap<HashString, DataSocket*>&, HashMap<HashString, DataSocket*>&, Vector<ExecutionSocket*>&, Vector<ExecutionSocket*>&)>;
		
		CustomNode(HashString name) : Node(name)
		{
		}

		CustomNode(CustomNode& other) = default;

		void OnProcess(CustomNodeCallback callback)
		{
			p_Callback = callback;
		}

		void Process() override
		{
			p_Callback(m_InputDataSockets, m_OutputDataSockets, m_InputExecutionSockets, m_OutputExecutionSockets);
		}


		Node* Clone() override
		{
			CustomNode* n = new CustomNode(*this);
			for (auto& [name, socket] : m_InputDataSockets)
			{
				n->m_InputDataSockets[name] = socket->Clone();
			}
			for (auto& [name, socket] : m_OutputDataSockets)
			{
				n->m_OutputDataSockets[name] = socket->Clone();
			}
			for (auto exe : m_InputExecutionSockets)
			{
				n->m_InputExecutionSockets.push_back(exe->CloneHeap());
			}
			for (auto exe : m_OutputExecutionSockets)
			{
				n->m_OutputExecutionSockets.push_back(exe->CloneHeap());
			}
			return n;
		}
	protected:

		CustomNodeCallback p_Callback = NULL;
	};


}