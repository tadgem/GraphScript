#include "GraphScript.h"

gs::HashString::HashString(const String& input) : m_Value(Hash(input)), m_Original(input) {
}

gs::HashString::HashString(const char* input) : m_Value(Hash(String(input))), m_Original(String(input)) {
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

gs::FunctionCallResult gs::GraphBuilder::CallFunction(HashString nameOfMethod, VariableSet args)
{
	if (m_Functions.find(nameOfMethod) == m_Functions.end())
	{
		return FunctionCallResult::Fail;
	}

	ResetSockets();
	
	IFunctionNode& func = m_Functions[nameOfMethod];
	PopulateParams(func, args);
	INode* next = &func;

	while (next != nullptr)
	{
		ProcessDataConnections();
		next->Process();

		next = FindRHS(next);
	}


	return FunctionCallResult::Success;
}

gs::INode* gs::GraphBuilder::FindRHS(INode* lhs)
{
	for (int i = 0; i < m_ExecutionConnections.size(); i++)
	{
		if (m_ExecutionConnections[i].m_LHS == lhs)
		{
			return m_ExecutionConnections[i].m_RHS;
		}
	}
	return nullptr;
}

gs::INode* gs::GraphBuilder::FindSocketNode(IDataSocketDef* socket)
{
	for (int i = 0; i < m_Nodes.size(); i++)
	{
		for (auto& [name, s] : m_Nodes[i]->m_InputDataSockets)
		{
			if (s.get() == socket)
			{
				return m_Nodes[i];
			}
		}
	}

	return nullptr;
}

void gs::GraphBuilder::PrintNodeSockets(INode* node)
{
}

void gs::GraphBuilder::ProcessDataConnections()
{
	const bool CONNECTIONS_DEBUG = false;

	if (CONNECTIONS_DEBUG)
	{
		std::cout << "--" << std::endl;
	}
	for (int i = 0; i < m_DataConnections.size(); i++)
	{
		if (CONNECTIONS_DEBUG)
		{
			m_DataConnections[i]->Print();
		}
		m_DataConnections[i]->Process();
	}
	if (CONNECTIONS_DEBUG)
	{
		std::cout << "--" << std::endl;
	}
}

void gs::GraphBuilder::PopulateParams(IFunctionNode& functionNode, VariableSet params)
{
	for (auto& [name, socket] : functionNode.m_OutputDataSockets)
	{
		if (params.find(name) != params.end())
		{
			socket->m_Value = params[name];
		}
	}
}

void gs::GraphBuilder::ResetSockets()
{
	for (INode* node : m_Nodes)
	{
		for (auto& [name, socket] : node->m_InputDataSockets)
		{
			socket->m_Value.reset();
		}

		for (auto& [name, socket] : node->m_OutputDataSockets)
		{
			socket->m_Value.reset();
		}
	}
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
	m_VariablesDefs.clear();
}