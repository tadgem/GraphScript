#include "GraphScript.h"

using namespace gs;

HashString::HashString(const String& input) : m_Value(Hash(input)), m_Original(input) {
}

HashString::HashString(const char* input) : m_Value(Hash(String(input))), m_Original(String(input)) {
}

HashString::HashString(u64 value) : m_Value(value)
{
}

u64 HashString::Hash(const String& input) {
	u64 r = 0;
	for (int i = 0; i < input.size(); i++) {
		char c = input[i];
		r ^= 397 * c;
	}
	return r;
}

FunctionNode& GraphBuilder::AddFunction(HashString functionName)
{
	// TODO: insert return statement here
	if (m_Functions.find(functionName) == m_Functions.end())
	{
		m_Functions[functionName] = CreateUnique<FunctionNode>();
		m_Nodes.push_back(m_Functions[functionName].get());
	}

	return *m_Functions[functionName];
}

void GraphBuilder::AddNode(Node* node)
{
	m_Nodes.push_back(node);
}

Graph GraphBuilder::Build()
{
	HashMap<HashString, FunctionNode*> functions = BuildFunctions();
	HashMap<HashString, Variable*> variables = BuildVariablesDefs();

	return { functions, variables, BuildNodes(functions), BuildExecutionConnections(functions), BuildDataConnections(functions, variables) };
}

ExecutionConnectionDef gs::GraphBuilder::ConnectExecutionSocket(ExecutionSocket* lhs, ExecutionSocket* rhs)
{
	// TODO: insert return statement here
	ExecutionConnectionDef conn{ lhs, rhs };
	m_ExecutionConnections.push_back(conn);
	return conn;
}

Node* GraphBuilder::FindSocketNode(DataSocket* socket)
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

void GraphBuilder::PrintNodeSockets(Node* node)
{
}

ExecutionSocket* gs::Graph::FindRHS(ExecutionSocket* lhs)
{
	for (auto conn : p_ExecutionConnections)
	{
		if (conn.m_LHS == lhs)
		{
			return conn.m_RHS;
		}
	}
	return nullptr;
}

GraphBuilder::~GraphBuilder()
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

HashMap<HashString, FunctionNode*> GraphBuilder::BuildFunctions()
{
	// Clone function defs
	HashMap<HashString, FunctionNode*> functions;
	for (auto& [name, func] : m_Functions)
	{
		functions[name] = (FunctionNode*) func->Clone();
	}
	return functions;
}

HashMap<HashString, Variable*> GraphBuilder::BuildVariablesDefs()
{
	// Clone variable defs
	HashMap<HashString, Variable*> variables;
	for (auto& [name, var] : m_VariablesDefs)
	{
		variables[name] = var->Clone();
	}
	return variables;
}

Vector<Node*> GraphBuilder::BuildNodes(HashMap<HashString, FunctionNode*>& functions)
{
	// Copy Nodes
	// Patch nodes removing function nodes replacing with clones
	Vector<Node*> nodes;
	for (Node* node : m_Nodes)
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
	return nodes;
}

Vector<ExecutionConnectionDef> GraphBuilder::BuildExecutionConnections(HashMap<HashString, FunctionNode*>& functions)
{
	// Copy Execution Connections
	// Patch Execution connection removing function nodes replacing with clones
	Vector<ExecutionConnectionDef> executionConnections;
	for (ExecutionConnectionDef& exec : m_ExecutionConnections)
	{
		ExecutionConnectionDef connection = exec;
		for (auto& [name, func] : m_Functions)
		{
			// this now needs to patch sockets on cloned nodes 
			// rather than the nodes them selves
			/*
			if (connection.m_LHS == func.get())
			{
				connection.m_LHS = functions[name];
			}
			if (connection.m_RHS == func.get())
			{
				connection.m_RHS = functions[name];
			}
			*/
		}

		executionConnections.push_back(connection);

	}
	return executionConnections;
}

Vector<DataConnection*> GraphBuilder::BuildDataConnections(HashMap<HashString, FunctionNode*>& functions,HashMap<HashString, Variable*> variables)
{
	// Copy Data Connections
	// Patch data connection removing function nodes replacing with clones
	Vector<DataConnection*> dataConnections;
	for (DataConnection* dataConn : m_DataConnections)
	{
		DataConnection* clone = dataConn->Clone();
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
	return dataConnections;
}

DataSocket::~DataSocket()
{
	m_Value.reset();
}

Node* gs::Graph::GetNode(ExecutionSocket* socket)
{
	for (auto node : p_Nodes)
	{
		for (auto inSocket : node->m_InputExecutionSockets)
		{
			if (inSocket == socket)
			{
				return node;
			}
		}

		for (auto outSocket : node->m_OutputExecutionSockets)
		{
			if (outSocket == socket)
			{
				return node;
			}
		}
	}
	return nullptr;
}

Graph::Graph(HashMap<HashString, FunctionNode*> functions, HashMap<HashString, Variable*> variablesDefs, Vector<Node*> nodes, Vector<ExecutionConnectionDef> executionConnections, Vector<DataConnection*> dataConnections) :
	p_Functions(functions), p_VariablesDefs(variablesDefs), p_Nodes(nodes), p_ExecutionConnections(executionConnections), p_DataConnections(dataConnections)
{
}

FunctionCallResult Graph::CallFunction(HashString nameOfMethod, VariableSet args)
{
	if (p_Functions.find(nameOfMethod) == p_Functions.end())
	{
		return FunctionCallResult::Fail;
	}

	// reset sockets so no lingering data from previous invoke
	ResetSockets();

	// Get the function entry node
	FunctionNode* func = p_Functions[nameOfMethod];

	// populate the args as params
	PopulateParams(func, args);

	Node* currentNode = func;
	ExecutionSocket *lhs, *rhs = nullptr;
	
	// get the first socket in the chain
	lhs = func->m_OutputExecutionSockets.front();

	// while there is something in the chain to process
	while (lhs != nullptr || rhs != nullptr)
	{
		// find RHS of lhs
		rhs = FindRHS(lhs);

		// invalidate lhs
		lhs = nullptr;

		Node* rhsNode = nullptr;

		if (rhs == nullptr)
		{
			// if we have elements on the stack
			if (!p_Stack.empty())
			{
				rhsNode = p_Stack.top();
				p_Stack.pop();
			}
			// otherwise finished
			else
			{
				continue;
			}			
		}
		else
		{
			rhsNode = GetNode(rhs);
		}


		if (rhsNode == nullptr)
		{
			continue;
		}

		// process data connections
		ProcessDataConnections();
		// Process()
		rhsNode->Process();

		// dont have to worry about loops or branches
		if (rhsNode->m_OutputExecutionSockets.size() == 1)
		{
			lhs = rhsNode->m_OutputExecutionSockets.front();
			continue;
		}

		// for each output pin (reverse)
		for (auto socket : rhsNode->m_OutputExecutionSockets)
		{
			if (socket->ShouldExecute())
			{
				lhs = socket;
				break;
			}
		}

		// seem to be hitting out here as executable before iter
		// is the process happening when we think it is?
		if (lhs && lhs->m_LoopCount > 1)
		{
			// read: the current node, the current output socket, and that sockets loop count
			p_Stack.push( rhsNode);
		}
	}

	return FunctionCallResult::Success;
}

void Graph::ProcessDataConnections()
{
	for (int i = 0; i < p_DataConnections.size(); i++)
	{
		p_DataConnections[i]->Process();
	}
}

void Graph::PopulateParams(FunctionNode* functionNode, VariableSet params)
{
	for (auto& [name, socket] : functionNode->m_OutputDataSockets)
	{
		if (params.find(name) != params.end())
		{
			socket->m_Value = params[name];
		}
	}
}

void Graph::ResetSockets()
{
	for (Node* node : p_Nodes)
	{
		for (auto& [name, socket] : node->m_InputDataSockets)
		{
			if (!socket) continue;
			
			socket->m_Value.reset();	
		}

		for (auto& [name, socket] : node->m_OutputDataSockets)
		{
			if (!socket) continue;
			
			socket->m_Value.reset();
		}
	}
}


ExecutionSocket::ExecutionSocket(HashString socketName, u32 loopCount) : m_SocketName(socketName), m_LoopCount(loopCount), p_ShouldExecute(true){}

ExecutionSocket* gs::Node::AddExecutionInput(HashString name)
{
	return m_InputExecutionSockets.emplace_back(new ExecutionSocket(name));
}

ExecutionSocket* gs::Node::AddExecutionOutput(HashString name)
{
	return m_OutputExecutionSockets.emplace_back(new ExecutionSocket(name));
}

gs::FunctionNode::FunctionNode()
{
	m_OutputExecutionSockets.push_back(new ExecutionSocket("out"));
}