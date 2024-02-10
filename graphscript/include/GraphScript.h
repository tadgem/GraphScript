//
// Created by Admin on 4/15/2023.
//

#ifndef GRAPHSCRIPT_GUARD_H
#define GRAPHSCRIPT_GUARD_H
#include <string>
#include <any>
#include <map>

namespace gs
{
	using String = std::string;

	template<typename T, typename Y>
	using HashMap = std::map<T, Y>;

	using Any = std::any;

	struct HashString {
		HashString() { m_Value = 0; }

		HashString(const String& input);
		HashString(uint64_t value);

		uint64_t m_Value;

		bool operator==(HashString const& rhs) const { return m_Value == rhs.m_Value; }

		operator uint64_t() const { return m_Value; }

	protected:
		static uint64_t Hash(const String& input);
	};
	
	using VariableSet = HashMap<HashString, Any>;
	
	class IDataSocketDef
	{

	};

	class IFunctionDef
	{
		HashMap<HashString, IDataSocketDef> m_DataSockets;
	};

	class IDataConnectionDef
	{

	};

	class IExecutionConnectionDef
	{

	};

	class IVariableDef
	{

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
		VariableSet m_Variables;

		FunctionCallResult CallFunction(const String& functionName, VariableSet args);
		FunctionCallResult CallFunction(HashString functionName, VariableSet args);
	};

	class GraphBuilder
	{
		IFunctionDef& AddFunction(const String& functionName);

		HashMap<HashString, IVariableDef> m_Variables;
		HashMap<HashString, IFunctionDef> m_Functions;

	};

	class Context
	{
		Graph* AddGraph(GraphBuilder* graph);
	};


}
#endif //GRAPHSCRIPT_GUARD_H
