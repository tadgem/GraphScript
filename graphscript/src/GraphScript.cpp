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
		m_Functions[functionName] = CreateUnique<IFunctionNode>();
		m_Nodes.push_back(m_Functions[functionName].get());
	}

	return *m_Functions[functionName];
}

void gs::GraphBuilder::AddNode(INode* node)
{
	m_Nodes.push_back(node);
}

gs::Graph gs::GraphBuilder::Build()
{
	// Clone function defs
	HashMap<HashString, IFunctionNode*> functions;
	for (auto& [name, func] : m_Functions)
	{
		functions[name] = func->Clone();
	}
	// Clone variable defs
	HashMap<HashString, IVariableDef*> variables;
	for (auto& [name, var] : m_VariablesDefs)
	{
		variables[name] = var->Clone();
	}

	// Copy Nodes
	// Patch nodes removing function nodes replacing with clones
	Vector<INode*> nodes;
	for (INode* node : m_Nodes)
	{
		bool pushed = false;
		for (auto& [name, func] : m_Functions)
		{
			// Replace the function nodes with updated clones
			if (func.get() == node)
			{
				nodes.push_back(functions[name]);
				pushed = true;
				continue;
			}
		}
		if (pushed)
		{
			continue;
		}

		nodes.push_back(node);
	}
	// Copy Execution Connections
	// Patch Execution connection removing function nodes replacing with clones
	Vector<IExecutionConnectionDef> executionConnections;
	for (IExecutionConnectionDef& exec : m_ExecutionConnections)
	{
		IExecutionConnectionDef connection = exec;
		for (auto& [name, func] : m_Functions)
		{
			if (connection.m_LHS == func.get())
			{
				connection.m_LHS = functions[name];
			}
			if (connection.m_RHS == func.get())
			{
				connection.m_RHS = functions[name];
			}
		}

		executionConnections.push_back(connection);

	}
	// Copy Data Connections
	// Patch data connection removing function nodes replacing with clones
	Vector<IDataConnectionDef*> dataConnections;
	for (IDataConnectionDef* dataConn : m_DataConnections)
	{
		IDataConnectionDef* clone = dataConn->Clone();
		for (auto& [name, func] : m_Functions)
		{
			for (auto& [outputName, output] : func->m_OutputDataSockets)
			{
				if (clone->m_LHS == output)
				{
					clone->m_LHS = functions[name]->m_OutputDataSockets[outputName];
				}

				if (clone->m_RHS == output)
				{
					clone->m_RHS = functions[name]->m_OutputDataSockets[outputName];
				}
			}
		}

		for (auto& [name, var] : m_VariablesDefs)
		{
			if (clone->m_LHS == var->GetSocket())
			{
				clone->m_LHS = variables[name]->GetSocket();
			}

			if (clone->m_RHS == var->GetSocket())
			{
				clone->m_RHS = variables[name]->GetSocket();
			}
		}
		dataConnections.push_back(clone);
	}


	return Graph{ functions, variables, nodes, executionConnections, dataConnections};
}

gs::IExecutionConnectionDef gs::GraphBuilder::ConnectNode(gs::INode* lhs, gs::INode* rhs)
{
	// TODO: insert return statement here
	IExecutionConnectionDef conn{ lhs, rhs };
	m_ExecutionConnections.push_back(conn);
	return conn;
}


gs::INode* gs::GraphBuilder::FindSocketNode(IDataSocketDef* socket)
{
	for (int i = 0; i < m_Nodes.size(); i++)
	{
		for (auto& [name, s] : m_Nodes[i]->m_InputDataSockets)
		{
			if (s == socket)
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

gs::IDataSocketDef::~IDataSocketDef()
{
	m_Value.reset();
}

gs::FunctionCallResult gs::Graph::CallFunction(HashString nameOfMethod, VariableSet args)
{
	if (m_Functions.find(nameOfMethod) == m_Functions.end())
	{
		return FunctionCallResult::Fail;
	}

	ResetSockets();

	IFunctionNode* func = m_Functions[nameOfMethod];
	PopulateParams(func, args);
	INode* next = func;

	while (next != nullptr)
	{
		ProcessDataConnections();
		next->Process();

		next = FindRHS(next);
	}


	return FunctionCallResult::Success;
}

gs::INode* gs::Graph::FindRHS(INode* lhs)
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

void gs::Graph::ProcessDataConnections()
{
	for (int i = 0; i < m_DataConnections.size(); i++)
	{
		m_DataConnections[i]->Process();
	}
}

void gs::Graph::PopulateParams(IFunctionNode* functionNode, VariableSet params)
{
	for (auto& [name, socket] : functionNode->m_OutputDataSockets)
	{
		if (params.find(name) != params.end())
		{
			socket->m_Value = params[name];
		}
	}
}

void gs::Graph::ResetSockets()
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
