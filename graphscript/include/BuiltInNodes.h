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
}