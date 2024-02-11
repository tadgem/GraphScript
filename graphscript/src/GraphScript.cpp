#include "GraphScript.h"

gs::HashString::HashString(const String& input) : m_Value(Hash(input)) {
}

gs::HashString::HashString(const char* input) : m_Value(Hash(String(input))) {
}

gs::HashString::HashString(uint64_t value) : m_Value(value)
{
}
uint64_t gs::HashString::Hash(const String& input) {
	uint64_t r = 0;
	for (int i = 0; i < input.size(); i++) {
		char c = input[i];
		r ^= 397 * c;
	}
	return r;
}

gs::IFunctionNode& gs::GraphBuilder::AddFunction(HashString functionName)
{
	// TODO: insert return statement here
	if (m_Functions.find(functionName) == m_Functions.end())
	{
		m_Functions[functionName] = IFunctionNode();
		m_Nodes.push_back(&m_Functions[functionName]);
	}

	return m_Functions[functionName];
}

void gs::GraphBuilder::AddNode(INode* node)
{
	m_Nodes.push_back(node);
}

gs::IExecutionConnectionDef gs::GraphBuilder::ConnectNode(gs::INode* lhs, gs::INode* rhs)
{
	// TODO: insert return statement here
	IExecutionConnectionDef conn{ lhs, rhs };
	m_ExecutionConnections.push_back(conn);
	return conn;
}


void gs::ICustomNode::Process()
{
	m_Proc();
}

gs::GraphBuilder::~GraphBuilder()
{
	for (int i = 0; i < m_DataConnections.size(); i++)
	{
		delete m_DataConnections[i];
	}
	m_DataConnections.clear();
	m_Functions.clear();
	m_Nodes.clear();
	m_Variables.clear();
}