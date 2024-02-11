//
// Created by Admin on 4/15/2023.
//

#ifndef GRAPHSCRIPT_GUARD_H
#define GRAPHSCRIPT_GUARD_H
#include "TypeDef.h"
namespace gs
{
	using VariableSet = HashMap<HashString, Any>;
	
	enum FunctionCallResult
	{
		Success,
		Fail
	};

	class IDataSocketDef
	{
	public:
		Any m_Value;
	};

	template <typename T>
	class IDataSocketDefT : public IDataSocketDef
	{
	public:
		T		Get()
		{
			return std::any_cast<T>(m_Value);
		}
		void Set(const T& other)
		{
			m_Value = other;
		}
	};

	class IDataConnectionDef
	{
	public:
		virtual void Process() = 0;
	};

	template<typename T>
	class IDataConnectionDefT : public IDataConnectionDef
	{
	public:
		IDataConnectionDefT(IDataSocketDefT<T>* lhs, IDataSocketDefT<T>* rhs)
		{
			p_LHS = lhs;
			p_RHS = rhs;
		}

		void Process() override
		{
			p_RHS->Set(p_LHS->Get());
		}

	protected:
		IDataSocketDefT<T>* p_LHS;
		IDataSocketDefT<T>* p_RHS;
	};



	class IFunctionDef
	{
	public:
		template <typename T>
		IDataSocketDefT<T>* AddArgument(HashString variableName)
		{
			if (m_DataSockets.find(variableName) == m_DataSockets.end())
			{
				m_DataSockets.emplace(variableName, CreateUnique<IDataSocketDefT<T>>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_DataSockets[variableName].get());
		}

		HashMap<HashString, Unique<IDataSocketDef>> m_DataSockets;
	};

	class IExecutionConnectionDef
	{

	};

	class IVariableDef
	{
	public:
		Any m_Value;
	};

	template <typename T>
	class IVariableDefT : public IVariableDef
	{
	public:
		T		Get()
		{
			T val = std::any_cast<T>(m_Value);
		}
		void Set(const T& other)
		{
			m_Value = other;
			m_Socket.Set(m_Value);
		}

		IDataSocketDefT<T> m_Socket;
	};

	class INode
	{
	public:
		virtual void Process() = 0;

	};

	class ICustomNode : public INode
	{
	public:
		template <typename T>
		IDataSocketDefT<T>* AddInput(HashString variableName)
		{
			if (m_InputDataSockets.find(variableName) == m_InputDataSockets.end())
			{
				m_InputDataSockets.emplace(variableName, CreateUnique<IDataSocketDefT<T>>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_InputDataSockets[variableName].get());
		}

		template <typename T>
		IDataSocketDefT<T>* AddOutput(HashString variableName)
		{
			if (m_OutputDataSockets.find(variableName) == m_OutputDataSockets.end())
			{
				m_OutputDataSockets.emplace(variableName, CreateUnique<IDataSocketDefT<T>>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_OutputDataSockets[variableName].get());
		}

		void AddFunctionality(Procedure proc)
		{
			m_Proc = proc;
		}

		void Process() override;

		HashMap<HashString, Unique<IDataSocketDef>> m_InputDataSockets;
		HashMap<HashString, Unique<IDataSocketDef>> m_OutputDataSockets;
		Procedure m_Proc = NULL;
	};

	class Graph
	{
	public:
		VariableSet m_Variables;

		FunctionCallResult CallFunction(const String& functionName, VariableSet args);
		FunctionCallResult CallFunction(HashString functionName, VariableSet args);
	};

	class GraphBuilder
	{
	public:

		~GraphBuilder();

		IFunctionDef& AddFunction(HashString functionName);

		template <typename T>
		IVariableDefT<T>* AddVariable(HashString variableName)
		{
			if (m_Variables.find(variableName) == m_Variables.end())
			{
				m_Variables.emplace(variableName, CreateUnique<IVariableDefT<T>>());
			}
			return static_cast<IVariableDefT<T>*>(m_Variables[variableName].get());
		}

		void AddNode(INode* node)
		{
			m_Nodes.push_back(node);
		}

		template<typename T>
		IDataConnectionDefT<T>* Connect(IDataSocketDefT<T>* lhs, IDataSocketDefT<T>* rhs)
		{
			auto conn = new IDataConnectionDefT<T>(lhs, rhs);
			m_Connections.emplace_back(conn);
			return conn;
		}

		HashMap<HashString, Unique<IVariableDef>> m_Variables;
		HashMap<HashString, IFunctionDef> m_Functions;
		Vector<INode*> m_Nodes;
		Vector<IDataConnectionDef*> m_Connections;
	};

	class Context
	{
		Graph* AddGraph(GraphBuilder* graph);
	};


}
#endif //GRAPHSCRIPT_GUARD_H
