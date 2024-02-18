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

IFunctionNode& GraphBuilder::AddFunction(HashString functionName)
{
	// TODO: insert return statement here
	if (m_Functions.find(functionName) == m_Functions.end())
	{
		m_Functions[functionName] = CreateUnique<IFunctionNode>();
		m_Nodes.push_back(m_Functions[functionName].get());
	}

	return *m_Functions[functionName];
}

void GraphBuilder::AddNode(INode* node)
{
	m_Nodes.push_back(node);
}

Graph GraphBuilder::Build()
{
	HashMap<HashString, IFunctionNode*> functions = BuildFunctions();
	HashMap<HashString, IVariableDef*> variables = BuildVariablesDefs();

	return Graph(functions, variables, BuildNodes(functions), BuildExecutionConnections(functions), BuildDataConnections(functions, variables));
}

IExecutionConnectionDef gs::GraphBuilder::ConnectExecutionSocket(IExecutionSocket* lhs, IExecutionSocket* rhs)
{
	// TODO: insert return statement here
	IExecutionConnectionDef conn{ lhs, rhs };
	m_ExecutionConnections.push_back(conn);
	return conn;
}

INode* GraphBuilder::FindSocketNode(IDataSocketDef* socket)
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

void GraphBuilder::PrintNodeSockets(INode* node)
{
}

IExecutionSocket* gs::Graph::FindRHS(IExecutionSocket* lhs)
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

INode* gs::Graph::GetNode(IExecutionSocket* socket)
{
	for (auto node : p_Nodes)
	{
		for (auto [socketName, inSocket] : node->m_InputExecutionSockets)
		{
			if (inSocket == socket)
			{
				return node;
			}
		}

		for (auto [socketName, outSocket] : node->m_OutputExecutionSockets)
		{
			if (outSocket == socket)
			{
				return node;
			}
		}
	}
	return nullptr;
}

void ICustomNode::Process()
{
	m_Proc();
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

IDataSocketDef::~IDataSocketDef()
{
	m_Value.reset();
}

Graph::Graph(HashMap<HashString, IFunctionNode*> functions, HashMap<HashString, IVariableDef*> variablesDefs, Vector<INode*> nodes, Vector<IExecutionConnectionDef> executionConnections, Vector<IDataConnectionDef*> dataConnections) :
	p_Functions(functions), p_VariablesDefs(variablesDefs), p_Nodes(nodes), p_ExecutionConnections(executionConnections), p_DataConnections(dataConnections)
{
}

FunctionCallResult Graph::CallFunction(HashString nameOfMethod, VariableSet args)
{
	if (p_Functions.find(nameOfMethod) == p_Functions.end())
	{
		return FunctionCallResult::Fail;
	}

	ResetSockets();

	IFunctionNode* func = p_Functions[nameOfMethod];
	PopulateParams(func, args);

	INode* currentNode = func;
	IExecutionSocket *lhs, *rhs = nullptr;
	// get the first socket in the chain
	lhs = func->m_OutputExecutionSockets["out"];
	// is this right? lhs likely never to be null 
	// so we can terminate the loop early by lhs = nullptr;
	while (lhs != nullptr || rhs != nullptr)
	{
		// find RHS of lhs
		rhs = FindRHS(lhs);
		if (rhs == nullptr)
		{
			lhs = nullptr;
			continue;
		}

		// find node of RHS
		INode* rhsNode = GetNode(rhs);
		// process data connections
		ProcessDataConnections();
		// Process()
		rhsNode->Process();

		// dont have to worry about loops or branches
		if (rhsNode->m_OutputExecutionSockets.size() == 1)
		{
			lhs = rhsNode->m_OutputExecutionSockets.begin()->second;
			continue;
		}

		lhs = nullptr;
		// we may have potentiall 
		// for each output pin (reverse)
		//     if (socket.ShouldExecute())
		//			lhs = socket.
		// we need a stack to return to this node when dealing with multiple output pins

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

void Graph::PopulateParams(IFunctionNode* functionNode, VariableSet params)
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
	for (INode* node : p_Nodes)
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

HashMap<HashString, IFunctionNode*> GraphBuilder::BuildFunctions()
{
	// Clone function defs
	HashMap<HashString, IFunctionNode*> functions;
	for (auto& [name, func] : m_Functions)
	{
		functions[name] = func->Clone();
	}
	return functions;
}

HashMap<HashString, IVariableDef*> GraphBuilder::BuildVariablesDefs()
{
	// Clone variable defs
	HashMap<HashString, IVariableDef*> variables;
	for (auto& [name, var] : m_VariablesDefs)
	{
		variables[name] = var->Clone();
	}
	return variables;
}

Vector<INode*> GraphBuilder::BuildNodes(HashMap<HashString, IFunctionNode*>& functions)
{
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
	return nodes;
}

Vector<IExecutionConnectionDef> GraphBuilder::BuildExecutionConnections(HashMap<HashString, IFunctionNode*>& functions)
{
	// Copy Execution Connections
	// Patch Execution connection removing function nodes replacing with clones
	Vector<IExecutionConnectionDef> executionConnections;
	for (IExecutionConnectionDef& exec : m_ExecutionConnections)
	{
		IExecutionConnectionDef connection = exec;
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

Vector<IDataConnectionDef*> GraphBuilder::BuildDataConnections(HashMap<HashString, IFunctionNode*>& functions,HashMap<HashString, IVariableDef*> variables)
{
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
	return dataConnections;
}

IExecutionSocket::IExecutionSocket(u32 loopCount) : p_LoopCount(loopCount)
{

}

bool IExecutionSocket::ShouldExecute()
{
	return p_ShouldExecute;
}

void IExecutionSocket::Execute()
{
	p_ShouldExecute = true;
}

IExecutionSocket* gs::ICustomNode::AddExecutionInput(HashString name)
{
	if (m_InputExecutionSockets.find(name) == m_InputExecutionSockets.end())
	{
		m_InputExecutionSockets[name] = new IExecutionSocket();
	}
	return m_InputExecutionSockets[name];
}

IExecutionSocket* gs::ICustomNode::AddExecutionOutput(HashString name)
{
	if (m_OutputExecutionSockets.find(name) == m_OutputExecutionSockets.end())
	{
		m_OutputExecutionSockets[name] = new IExecutionSocket();
	}
	return m_OutputExecutionSockets[name];
}

gs::IFunctionNode::IFunctionNode()
{
	m_OutputExecutionSockets["out"] = new IExecutionSocket();
}
