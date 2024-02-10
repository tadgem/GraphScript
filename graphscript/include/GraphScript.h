//
// Created by Admin on 4/15/2023.
//

#ifndef GRAPHSCRIPT_GUARD_H
#define GRAPHSCRIPT_GUARD_H
#include "TypeDef.h"
namespace gs
{
	using VariableSet = HashMap<HashString, Any>;
	
	class IDataSocketDef
	{
	public:
	};

	template <typename T>
	class IDataSocketDefT : public IDataSocketDef
	{
	public:
		T& Get();
		bool	Set(const T& other);
	};


	class IFunctionDef
	{
	public:
		template <typename T>
		IDataSocketDef* AddArgument(HashString variableName)
		{
			if (m_DataSockets.find(variableName) == m_DataSockets.end())
			{
				m_DataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return m_DataSockets[variableName];
		}

		HashMap<HashString, IDataSocketDef*> m_DataSockets;
	};

	class IDataConnectionDef
	{

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
		IDataSocketDef* AddInput(HashString variableName)
		{
			if (m_InputDataSockets.find(variableName) == m_InputDataSockets.end())
			{
				m_InputDataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return m_InputDataSockets[variableName];
		}

		template <typename T>
		IDataSocketDef* AddOutput(HashString variableName)
		{
			if (m_OutputDataSockets.find(variableName) == m_OutputDataSockets.end())
			{
				m_OutputDataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return m_OutputDataSockets[variableName];
		}

		HashMap<HashString, IDataSocketDef*> m_InputDataSockets;
		HashMap<HashString, IDataSocketDef*> m_OutputDataSockets;
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
		IVariableDef* AddVariable(HashString variableName)
		{
			if (m_Variables.find(variableName) == m_Variables.end())
			{
				m_Variables.emplace(variableName, new IVariableDefT<T>());
			}
			return m_Variables[variableName];
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
