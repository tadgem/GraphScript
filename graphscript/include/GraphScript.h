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

	};

	template <typename T>
	class IDataSocketDefT : IDataSocketDef
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
			if (m_DataSockets.find(variableName) != m_DataSockets.end())
			{
				return m_DataSockets[variableName];
			}

			m_DataSockets.emplace(variableName, new IDataSocketDefT<T>());
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
	class IVariableDefT : IVariableDef
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
		IFunctionDef& AddFunction(const String& functionName);

		template <typename T>
		IVariableDef* AddVariable(HashString variableName)
		{
			if (m_Variables.find(variableName) != m_Variables.end())
			{
				return m_Variables[variableName];
			}

			m_Variables.emplace(variableName, new IVariableDefT<T>());
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
