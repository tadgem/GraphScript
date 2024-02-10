//
// Created by Admin on 4/15/2023.
//

#ifndef GRAPHSCRIPT_GUARD_H
#define GRAPHSCRIPT_GUARD_H
#include "TypeDef.h"
namespace gs
{
	using VariableSet = HashMap<HashString, Any>;
	
	class IDataConnectionDef
	{
	public:
	};

	template<typename T>
	class IDataConnectionDefT : public IDataConnectionDef
	{
	public:

	};

	class IDataSocketDef
	{
	public:
	};

	template <typename T>
	class IDataSocketDefT : public IDataSocketDef
	{
	public:
		Optional<T>		Get()
		{
			return {};
		}
		bool	Set(const T& other)
		{
			return false;
		}

		IDataConnectionDefT<T>* m_Connection = nullptr;
	};


	class IFunctionDef
	{
	public:
		template <typename T>
		IDataSocketDefT<T>* AddArgument(HashString variableName)
		{
			if (m_DataSockets.find(variableName) == m_DataSockets.end())
			{
				m_DataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_DataSockets[variableName]);
		}

		HashMap<HashString, IDataSocketDef*> m_DataSockets;
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
		T&		Get();
		bool	Set(const T& other);
	};

	class INode
	{

	};

	class INodeBuilder
	{
	public:
		template <typename T>
		IDataSocketDefT<T>* AddInput(HashString variableName)
		{
			if (m_InputDataSockets.find(variableName) == m_InputDataSockets.end())
			{
				m_InputDataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_InputDataSockets[variableName]);
		}

		template <typename T>
		IDataSocketDefT<T>* AddOutput(HashString variableName)
		{
			if (m_OutputDataSockets.find(variableName) == m_OutputDataSockets.end())
			{
				m_OutputDataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_OutputDataSockets[variableName]);
		}

		void AddFunctionality(Procedure proc)
		{
			m_Proc = proc;
		}

		HashMap<HashString, IDataSocketDef*> m_InputDataSockets;
		HashMap<HashString, IDataSocketDef*> m_OutputDataSockets;
		Procedure m_Proc = NULL;
	};


	enum FunctionCallResult
	{
		Success,
		Fail
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
		IFunctionDef& AddFunction(HashString functionName);

		template <typename T>
		IVariableDefT<T>* AddVariable(HashString variableName)
		{
			if (m_Variables.find(variableName) == m_Variables.end())
			{
				m_Variables.emplace(variableName, new IVariableDefT<T>());
			}
			return static_cast<IVariableDefT<T>*>(m_Variables[variableName]);
		}

		HashMap<HashString, IVariableDef*> m_Variables;
		HashMap<HashString, IFunctionDef> m_Functions;

	};

	class Context
	{
		Graph* AddGraph(GraphBuilder* graph);
	};


}
#endif //GRAPHSCRIPT_GUARD_H
