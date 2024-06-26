#include "GraphScript.h"
#include "BuiltInNodes.h"
using namespace gs;

Vector<String> SplitStringByChar(const String& str, char c)
{
	auto result = Vector<String>{};
	auto ss = SStream{ str };

	for (String line; std::getline(ss, line, c);)
		result.push_back(line);

	return result;
}

void Trim(String& s, char c) {

	s.erase(std::remove_if(s.begin(), s.end(), isspace), s.end());
}

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
		m_Functions[functionName] = CreateUnique<FunctionNode>(functionName);
		m_Nodes.push_back(m_Functions[functionName].get());
	}

	return *m_Functions[functionName];
}

void GraphBuilder::AddNode(Node* node)
{
	m_Nodes.push_back(node);
}

void GraphBuilder::DeleteNode(int index)
{
	if (index < 0)
	{
		return;
	}

	Node* n = m_Nodes[index];

	Vector<int> dataConnectionsToRelease;

	for (int i = 0; i < m_DataConnections.size(); i++)
	{
		if (FindDataSocketNode(m_DataConnections[i]->m_LHS) == n ||
			FindDataSocketNode(m_DataConnections[i]->m_RHS) == n)
		{
			delete m_DataConnections[i];
			dataConnectionsToRelease.push_back(i);
		}
	}

	for (int i = static_cast<int>(dataConnectionsToRelease.size() - 1); i >= 0; i--)
	{
		int index = dataConnectionsToRelease[i];
		m_DataConnections.erase(m_DataConnections.begin() + index);
	}

	Vector<int> exeConnectionsToRelease;

	for (int i = 0; i < m_ExecutionConnections.size(); i++)
	{
		if (FindExeSocketNode(m_ExecutionConnections[i].m_LHS) == n ||
			FindExeSocketNode(m_ExecutionConnections[i].m_RHS) == n)
		{
			exeConnectionsToRelease.push_back(i);
		}
	}

	for (int i = static_cast<int>(exeConnectionsToRelease.size()) - 1; i >= 0; i--)
	{
		int index = exeConnectionsToRelease[i];
		m_ExecutionConnections.erase(m_ExecutionConnections.begin() + index);
	}

	delete n;
	m_Nodes.erase(m_Nodes.begin() + index);

}

Graph* GraphBuilder::Build()
{
	// clone functions
	HashMap<HashString, FunctionNode*> functions = BuildFunctions();
	// clone variable defs
	HashMap<HashString, Variable*> variables = BuildVariables();
	// clone all non function nodes
	Vector<Node*> nodes = BuildNodes(functions);
	// map execution connections from builder to cloned node sockets
	Vector<ExecutionConnectionDef> executionConns = BuildExecutionConnections(functions, nodes);
	// clone all data connections and map builder lhs and rhs to cloned node sockets
	Vector<DataConnection*> dataConns = BuildDataConnections(functions, variables, nodes);

	return new Graph{ m_Name, functions, variables, nodes, executionConns, dataConns };
}

DataConnection* gs::GraphBuilder::ConnectDataSocket(DataSocket* lhs, DataSocket* rhs)
{
	Type lhs_t = lhs->m_Type;
	Type rhs_t = rhs->m_Type;

	if (lhs_t.m_TypeHash != rhs_t.m_TypeHash)
	{
		return nullptr;
	}

	DataConnection* proto = p_Context->GetDataConnectionFromHash(lhs_t.m_TypeHash);
	if (proto)
	{
		auto c = proto->Clone();
		c->m_LHS = lhs;
		c->m_RHS = rhs;
		m_DataConnections.push_back(c);
		return c;
	}
	return nullptr;
}

void gs::GraphBuilder::DeleteVariable(HashString variableName)
{
	if (m_Variables.find(variableName) == m_Variables.end())
	{
		return;
	}

	Variable* var = m_Variables[variableName].get();

	Vector<int> dataConnectionsToRelease;

	for (int i = 0; i < m_DataConnections.size(); i++)
	{
		if (m_DataConnections[i]->m_LHS == var->GetSocket() ||
			m_DataConnections[i]->m_RHS == var->GetSocket())
		{
			delete m_DataConnections[i];
			dataConnectionsToRelease.push_back(i);
		}
	}

	for (int i = static_cast<int>(dataConnectionsToRelease.size() - 1); i >= 0; i--)
	{
		int index = dataConnectionsToRelease[i];
		m_DataConnections.erase(m_DataConnections.begin() + index);
	}
	
	m_Variables.erase(variableName);
}

void gs::GraphBuilder::DestroyDataConnection(int index)
{
	if (index < 0)
	{
		return;
	}

	delete m_DataConnections[index];

	m_DataConnections.erase(m_DataConnections.begin() + index);
}

ExecutionConnectionDef gs::GraphBuilder::ConnectExecutionSocket(ExecutionSocket* lhs, ExecutionSocket* rhs)
{
	// TODO: insert return statement here
	ExecutionConnectionDef conn{ lhs, rhs };
	m_ExecutionConnections.push_back(conn);
	return conn;
}

void gs::GraphBuilder::DestroyExecutionConnection(int index)
{
	if (index < 0)
	{
		return;
	}

	m_ExecutionConnections.erase(m_ExecutionConnections.begin() + index);
}

String utils::AnyToString(Any& any)
{
	if (!any.has_value())
	{
		return String();
	}
	if (any.type() == typeid(i8))
	{
		return std::to_string(std::any_cast<i8>(any));
	}

	if (any.type() == typeid(i16))
	{
		return std::to_string(std::any_cast<i16>(any));
	}

	if (any.type() == typeid(i32))
	{
		return std::to_string(std::any_cast<i32>(any));
	}

	if (any.type() == typeid(i64))
	{
		return std::to_string(std::any_cast<i64>(any));
	}

	if (any.type() == typeid(u8))
	{
		return std::to_string(std::any_cast<u8>(any));
	}

	if (any.type() == typeid(u16))
	{
		return std::to_string(std::any_cast<u16>(any));
	}

	if (any.type() == typeid(u32))
	{
		return std::to_string(std::any_cast<u32>(any));
	}

	if (any.type() == typeid(u64))
	{
		return std::to_string(std::any_cast<u64>(any));
	}

	if (any.type() == typeid(f32))
	{
		return std::to_string(std::any_cast<f32>(any));
	}

	if (any.type() == typeid(f64))
	{
		return std::to_string(std::any_cast<f64>(any));
	}

	if (any.type() == typeid(bool))
	{
		return std::to_string(std::any_cast<bool>(any));
	}

	if (any.type() == typeid(String))
	{
		return std::any_cast<String>(any);
	}

	return "";
}

Any utils::StringToAny(String& str, u64 typeHash)
{
	if (str.empty())
	{
		return {};
	}

	if (typeHash == HashString("unsigned long long"))
	{
		return Any{ std::stoull(str) };
	}

	if (typeHash == HashString("unsigned long"))
	{
		return Any{ std::stoul(str) };
	}

	if (typeHash == HashString("unsigned short"))
	{
		return Any{ (u16)std::stoul(str) };
	}

	if (typeHash == HashString("unsigned char"))
	{
		return Any{ (u8)std::stoul(str) };
	}

	if (typeHash == HashString("long long"))
	{
		return Any{ std::stoll(str) };
	}

	if (typeHash == HashString("long"))
	{
		return Any{ std::stol(str) };
	}

	if (typeHash == HashString("short"))
	{
		return Any{ (short)std::stoi(str) };
	}

	if (typeHash == HashString("char"))
	{
		return Any{ (char)std::stoi(str) };
	}

	if (typeHash == HashString("float"))
	{
		return Any{ std::stof(str) };
	}

	if (typeHash == HashString("double"))
	{
		return Any{ std::stod(str) };
	}

	if (typeHash == HashString("bool"))
	{
		return Any{ (bool)std::stoi(str) };
	}

	if (typeHash == GetTypeHash<String>())
	{
		return Any{ str };
	}

	return {};
}

String gs::GraphBuilder::Serialize()
{
	SStream stream;
	stream << "BeginName\n";
	stream << "    " << m_Name.m_Original;
	stream << "\nEndName\nBeginFunctions\n";
	for (auto& [name, func] : m_Functions)
	{
		stream << "    " << name.m_Original;
		for (auto& [name, socket] : func->m_OutputDataSockets)
		{
			stream << "," << name.m_Original << ":" << socket->m_Type.m_TypeHash;
		}
		stream << "\n";
	}

	stream << "EndFunctions\nBeginNodes\n";
	for (int i = 0; i < m_Nodes.size(); i++)
	{
		stream << "    " << m_Nodes[i]->m_NodeName.m_Original << "\n";
	}

	stream << "EndNodes\nBeginVariables\n";
	for (auto& [name, var] : m_Variables)
	{
		stream << "    " << name.m_Original << "," << var->m_Type.m_TypeHash.m_Value << "\n";
	}

	stream << "EndVariables\nBeginNodeDataConns\n";
	for (int i = 0; i < m_DataConnections.size(); i++)
	{
		DataSocket* lhs = m_DataConnections[i]->m_LHS;
		DataSocket* rhs = m_DataConnections[i]->m_RHS;
		i32 lhsNodeIndex = GetNodeIndex(FindDataSocketNode(lhs));
		i32 rhsNodeIndex = GetNodeIndex(FindDataSocketNode(rhs));

		if (lhsNodeIndex < 0 || rhsNodeIndex < 0)
		{
			continue;
		}

		stream << "    " << lhsNodeIndex << "," << FindDataSocketName(lhs).m_Original
			<< "," << lhs->m_Type.m_TypeHash.m_Value << ":";
		stream << rhsNodeIndex << "," << FindDataSocketName(rhs).m_Original
			<< "," << rhs->m_Type.m_TypeHash.m_Value << "\n";

	}

	stream << "EndNodeDataConns\nBeginVariableDataConns\n";
	for (int i = 0; i < m_DataConnections.size(); i++)
	{
		DataSocket* lhs = m_DataConnections[i]->m_LHS;

		HashString lhsVariableName = FindSocketVariableName(lhs);
		if (lhsVariableName.m_Value == 0)
		{
			continue;
		}

		DataSocket* rhs = m_DataConnections[i]->m_RHS;
		i32 rhsNodeIndex = GetNodeIndex(FindDataSocketNode(rhs));

		stream << "    " << lhsVariableName.m_Original << "," << lhs->m_Type.m_TypeHash.m_Value << ":";
		stream << rhsNodeIndex << "," << FindDataSocketName(rhs).m_Original
			<< "," << rhs->m_Type.m_TypeHash.m_Value << "\n";

	}

	stream << "EndVariableDataConns\nBeginExeConns\n";
	for (int i = 0; i < m_ExecutionConnections.size(); i++)
	{
		ExecutionSocket* lhs = m_ExecutionConnections[i].m_LHS;
		ExecutionSocket* rhs = m_ExecutionConnections[i].m_RHS;
		stream << "    " << GetNodeIndex(FindExeSocketNode(lhs)) << "," << lhs->m_SocketName.m_Original << ":";
		stream << GetNodeIndex(FindExeSocketNode(rhs)) << "," << rhs->m_SocketName.m_Original << "\n";
	}

	stream << "EndExeConns\nBeginDefaultValues\n";
	for (auto& [name, var] : m_Variables)
	{
		if (var->m_Value.has_value())
		{
			String anyValue = utils::AnyToString(var->m_Value);
			if (anyValue.size() == 0)
			{
				continue;
			}
			stream << name.m_Original << ":" << var->m_Type.m_TypeHash.m_Value << ":" << anyValue << std::endl;
		}
	}
	stream << "EndDefaultValues\n";
	return stream.str();
}

Node* GraphBuilder::FindDataSocketNode(DataSocket* socket)
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

		for (auto& [name, s] : m_Nodes[i]->m_OutputDataSockets)
		{
			if (s == socket)
			{
				return m_Nodes[i];
			}
		}
	}

	return nullptr;
}

Node* gs::GraphBuilder::FindExeSocketNode(ExecutionSocket* socket)
{
	for (int i = 0; i < m_Nodes.size(); i++)
	{
		for (auto& s : m_Nodes[i]->m_InputExecutionSockets)
		{
			if (s == socket)
			{
				return m_Nodes[i];
			}
		}

		for (auto& s : m_Nodes[i]->m_OutputExecutionSockets)
		{
			if (s == socket)
			{
				return m_Nodes[i];
			}
		}
	}

	return nullptr;
}

HashString gs::GraphBuilder::FindSocketVariableName(DataSocket* socket)
{
	for (auto& [name, var] : m_Variables)
	{
		if (var->GetSocket() == socket)
		{
			return name;
		}
	}
	return HashString();
}

HashString gs::GraphBuilder::FindDataSocketName(DataSocket* socket)
{
	Node* n = FindDataSocketNode(socket);
	if (!n)
	{
		return HashString();
	}

	for (auto& [name, s] : n->m_InputDataSockets)
	{
		if (s == socket)
		{
			return name;
		}
	}

	for (auto& [name, s] : n->m_OutputDataSockets)
	{
		if (s == socket)
		{
			return name;
		}
	}

	return HashString();
}

i32 gs::GraphBuilder::GetNodeIndex(Node* node)
{
	for (int i = 0; i < m_Nodes.size(); i++)
	{
		if (m_Nodes[i] == node)
		{
			return i;
		}
	}
	return -1;
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
	m_Variables.clear();
}

gs::GraphBuilder::GraphBuilder(Context* context, HashString name) : p_Context(context), m_Name(name)
{
}

HashMap<HashString, FunctionNode*> GraphBuilder::BuildFunctions()
{
	// Clone function defs
	HashMap<HashString, FunctionNode*> functions;
	for (auto& [name, func] : m_Functions)
	{
		functions[name] = (FunctionNode*)func->Clone();
	}
	return functions;
}

HashMap<HashString, Variable*> GraphBuilder::BuildVariables()
{
	// Clone variable defs
	HashMap<HashString, Variable*> variables;
	for (auto& [name, var] : m_Variables)
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

		nodes.push_back(node->Clone());
	}
	return nodes;
}

Vector<ExecutionConnectionDef> GraphBuilder::BuildExecutionConnections(HashMap<HashString, FunctionNode*>& functions, Vector<Node*> nodes)
{
	// Copy Execution Connections
	// Patch Execution connection removing function nodes replacing with clones
	Vector<ExecutionConnectionDef> executionConnections;
	for (ExecutionConnectionDef& exec : m_ExecutionConnections)
	{
		ExecutionConnectionDef connection = exec;
		for (auto& [name, func] : m_Functions)
		{
			for (int i = 0; i < func->m_OutputExecutionSockets.size(); i++)
			{
				if (connection.m_LHS == func->m_OutputExecutionSockets[i])
				{
					connection.m_LHS = functions[name]->m_OutputExecutionSockets[i];
				}
				if (connection.m_RHS == func->m_OutputExecutionSockets[i])
				{
					connection.m_RHS = functions[name]->m_OutputExecutionSockets[i];
				}
			}
		}

		for (int i = 0; i < m_Nodes.size(); i++)
		{
			for (int s = 0; s < m_Nodes[i]->m_InputExecutionSockets.size(); s++)
			{
				if (connection.m_LHS == m_Nodes[i]->m_InputExecutionSockets[s])
				{
					connection.m_LHS = nodes[i]->m_InputExecutionSockets[s];
				}

				if (connection.m_RHS == m_Nodes[i]->m_InputExecutionSockets[s])
				{
					connection.m_RHS = nodes[i]->m_InputExecutionSockets[s];
				}
			}

			for (int s = 0; s < m_Nodes[i]->m_OutputExecutionSockets.size(); s++)
			{
				if (connection.m_LHS == m_Nodes[i]->m_OutputExecutionSockets[s])
				{
					connection.m_LHS = nodes[i]->m_OutputExecutionSockets[s];
				}

				if (connection.m_RHS == m_Nodes[i]->m_OutputExecutionSockets[s])
				{
					connection.m_RHS = nodes[i]->m_OutputExecutionSockets[s];
				}
			}
		}

		executionConnections.push_back(connection);

	}
	return executionConnections;
}

Vector<DataConnection*> GraphBuilder::BuildDataConnections(HashMap<HashString, FunctionNode*>& functions, HashMap<HashString, Variable*> variables, Vector<Node*> nodes)
{
	// Copy Data Connections
	// Patch data connection removing function nodes replacing with clones
	Vector<DataConnection*> dataConnections;
	for (DataConnection* dataConn : m_DataConnections)
	{
		DataConnection* clone = dataConn->Clone();

		for (int i = 0; i < m_Nodes.size(); i++)
		{
			Node* node = m_Nodes[i];

			for (auto& [outputName, output] : node->m_InputDataSockets)
			{
				if (clone->m_LHS == output)
				{
					clone->m_LHS = nodes[i]->m_InputDataSockets[outputName];
				}
				if (clone->m_RHS == output)
				{
					clone->m_RHS = nodes[i]->m_InputDataSockets[outputName];
				}
			}

			for (auto& [outputName, output] : node->m_OutputDataSockets)
			{
				if (clone->m_LHS == output)
				{
					clone->m_LHS = nodes[i]->m_OutputDataSockets[outputName];
				}
				if (clone->m_RHS == output)
				{
					clone->m_RHS = nodes[i]->m_OutputDataSockets[outputName];
				}
			}
		}

		for (auto& [name, var] : m_Variables)
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

Graph::Graph(HashString name,HashMap<HashString, FunctionNode*> functions, HashMap<HashString, Variable*> variablesDefs, Vector<Node*> nodes, Vector<ExecutionConnectionDef> executionConnections, Vector<DataConnection*> dataConnections) :
	m_Name (name), p_Functions(functions), p_Variables(variablesDefs), p_Nodes(nodes), p_ExecutionConnections(executionConnections), p_DataConnections(dataConnections)
{
}

gs::Graph::~Graph()
{
	for (auto& [name, func] : p_Functions)
	{

		for (int i = (int)p_Nodes.size() - 1; i >= 0; i--)
		{
			if (p_Nodes[i] == func)
			{
				p_Nodes.erase(p_Nodes.begin() + i);
			}
		}

		delete func;
	}
	p_Functions.clear();

	for (auto& [name, var] : p_Variables)
	{
		delete var;
	}
	p_Variables.clear();

	for (int i = 0; i < p_Nodes.size(); i++)
	{
		delete p_Nodes[i];
	}
	p_Nodes.clear();

	for (int i = 0; i < p_DataConnections.size(); i++)
	{
		delete p_DataConnections[i];
	}
	p_DataConnections.clear();
	p_ExecutionConnections.clear();
}

FunctionCallResult Graph::CallFunction(HashString nameOfMethod, VariableSet args)
{
	if (p_Functions.find(nameOfMethod) == p_Functions.end())
	{
		return FunctionCallResult::Fail;
	}

	// reset sockets so no lingering data from previous invoke
	// ResetSockets();

	// Get the function entry node
	FunctionNode* func = p_Functions[nameOfMethod];

	// populate the args as params
	PopulateParams(func, args);

	Node* currentNode = func;
	ExecutionSocket* lhs, * rhs = nullptr;

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

		// this condition fails at the loop now
		if (lhs && lhs->m_LoopCount > 1)
		{
			// read: the current node, the current output socket, and that sockets loop count
			p_Stack.push(rhsNode);
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

ExecutionSocket::ExecutionSocket(HashString socketName, u32 loopCount) : m_SocketName(socketName), m_LoopCount(loopCount), p_ShouldExecute(true) {}

gs::Node::~Node()
{
	for (auto& [name, socket] : m_InputDataSockets)
	{
		delete socket;
	}
	m_InputDataSockets.clear();

	for (auto& [name, socket] : m_OutputDataSockets)
	{
		delete socket;
	}
	m_OutputDataSockets.clear();

	for (int i = 0; i < m_InputExecutionSockets.size(); i++)
	{
		delete m_InputExecutionSockets[i];
	}
	m_InputExecutionSockets.clear();

	for (int i = 0; i < m_OutputExecutionSockets.size(); i++)
	{
		delete m_OutputExecutionSockets[i];
	}
	m_OutputExecutionSockets.clear();
}

ExecutionSocket* gs::Node::AddExecutionInput(HashString name)
{
	return m_InputExecutionSockets.emplace_back(new ExecutionSocket(name));
}

ExecutionSocket* gs::Node::AddExecutionOutput(HashString name)
{
	return m_OutputExecutionSockets.emplace_back(new ExecutionSocket(name));
}

gs::FunctionNode::FunctionNode(HashString name) : gs::Node(name)
{
	m_OutputExecutionSockets.push_back(new ExecutionSocket("out"));
}

gs::Context::Context()
{
	AddBuiltIns();
}

GraphBuilder* gs::Context::CreateBuilder(HashString name)
{
	p_Builders.push_back(CreateUnique<GraphBuilder>(this, name));
	return p_Builders[p_Builders.size() - 1].get();
}

GraphBuilder* gs::Context::FindBuilder(HashString name)
{
	for (int i = 0; i < p_Builders.size(); i++)
	{
		if (p_Builders[i]->m_Name == name)
		{
			return p_Builders[i].get();
		}
	}
	return nullptr;
}

GraphBuilder* gs::Context::DeserializeGraph(String& source)
{
	Parser p(*this);

	p_Builders.push_back(p.Parse(source));

	return p_Builders[p_Builders.size() - 1].get();
}

Graph* gs::Context::BuildGraph(GraphBuilder* builder)
{
	Graph* g = builder->Build();

	p_ActiveGraphs.push_back(g);

	return g;
}

void gs::Context::DestroyGraph(Graph* graph)
{
	int index = -1;
	for (int i = 0; i < p_ActiveGraphs.size(); i++)
	{
		if (p_ActiveGraphs[i] == graph)
		{
			index = i;
		}
	}

	if (index >= 0)
	{
		delete p_ActiveGraphs[index];
		p_ActiveGraphs.erase(p_ActiveGraphs.begin() + index);
	}
}

void gs::Context::AddNode(Node* node)
{
	if (node == nullptr)
	{
		return;
	}

	p_Nodes.push_back(node);
}

void gs::Context::AddBuiltIns()
{
	RegisterType<f32>("float");
	RegisterType<bool>("bool");
	RegisterType<u32>("unsigned int");
	RegisterType<i32>("int");
	RegisterType<String>("string");
	RegisterType<Graph*>("graph");
	RegisterType<HashString>("hashstring");

	RegisterArithmaticFunctions<f32>("float");
	RegisterArithmaticFunctions<u32>("unsigned int");
	RegisterArithmaticFunctions<i32>("int");

	auto forNodeBuilder = new ForNode();
	auto whileNodeBuilder = new WhileNode();
	auto ifNodeBuilder = new IfNode();
	auto printf32NodeBuilder = new PrintNodeT<f32>("Print(float)");
	auto printu32NodeBuilder = new PrintNodeT<u32>("Print(unsigned int)");
	auto printi32NodeBuilder = new PrintNodeT<i32>("Print(int)");
	auto printBoolNodeBuilder = new PrintNodeT<bool>("Print(bool)");
	auto printStringNodeBuilder = new PrintNodeT<String>("Print(String)");

	AddNode(forNodeBuilder);
	AddNode(whileNodeBuilder);
	AddNode(ifNodeBuilder);
	AddNode(printf32NodeBuilder);
	AddNode(printu32NodeBuilder);
	AddNode(printi32NodeBuilder);
	AddNode(printBoolNodeBuilder);
	AddNode(printStringNodeBuilder);
}

gs::Context::Parser::Parser(Context& c) : p_Context(c)
{
}

Unique<GraphBuilder> gs::Context::Parser::Parse(String& source)
{
	Vector<String> lines = SplitStringByChar(source, '\n');
	Unique<GraphBuilder> graphBuilder = CreateUnique<GraphBuilder>(&p_Context, "EMPTY");

	State s = State::Invalid;

	for (String& l : lines)
	{
		State previous = s;
		Trim(l, ' ');
		HandleCurrentState(s, l);
		if (s == State::Invalid)
		{
			continue;
		}

		if (previous == State::Invalid)
		{
			continue;
		}

		switch (s)
		{
		case Name:
			ParseName(graphBuilder.get(), l);
			break;
		case Functions:
			ParseFunction(graphBuilder.get(), l);
			break;
		case Nodes:
			ParseNode(graphBuilder.get(), l);
			break;
		case Variables:
			ParseVariable(graphBuilder.get(), l);
			break;
		case NodeDataConnections:
			ParseNodeDataConnection(graphBuilder.get(), l);
			break;
		case VariableDataConnections:
			ParseVariableDataConnection(graphBuilder.get(), l);
			break;
		case ExecutionConnections:
			ParseExecutionConnection(graphBuilder.get(), l);
			break;
		case DefaultVariableValues:
			ParseDefaultValues(graphBuilder.get(), l);
			break;
		default:
			std::cout << "How did I get here?" << std::endl;
			continue;
		}
	}

	return graphBuilder;
}

void gs::Context::Parser::HandleCurrentState(State& s, String& l)
{
	if (l == "BeginName")
	{
		s = State::Name;
	}
	if (l == "EndName")
	{
		s = State::Invalid;
	}
	if (l == "BeginFunctions")
	{
		s = State::Functions;
	}
	if (l == "EndFunctions")
	{
		s = State::Invalid;
	}
	if (l == "BeginNodes")
	{
		s = State::Nodes;
	}
	if (l == "EndNodes")
	{
		s = State::Invalid;
	}
	if (l == "BeginVariables")
	{
		s = State::Variables;
	}
	if (l == "EndVariables")
	{
		s = State::Invalid;
	}
	if (l == "BeginNodeDataConns")
	{
		s = State::NodeDataConnections;
	}
	if (l == "EndNodeDataConns")
	{
		s = State::Invalid;
	}
	if (l == "BeginVariableDataConns")
	{
		s = State::VariableDataConnections;
	}
	if (l == "EndVariableDataConns")
	{
		s = State::Invalid;
	}
	if (l == "BeginExeConns")
	{
		s = State::ExecutionConnections;
	}
	if (l == "EndExeConns")
	{
		s = State::Invalid;
	}
	if (l == "BeginDefaultValues")
	{
		s = State::DefaultVariableValues;
	}
	if (l == "EndDefaultValues")
	{
		s = State::Invalid;
	}
}

void gs::Context::Parser::ParseName(GraphBuilder* builder, String& line)
{
	builder->m_Name = line;
}

void gs::Context::Parser::ParseFunction(GraphBuilder* builder, String& line)
{
	Vector<String> lineContents = SplitStringByChar(line, ',');
	GS_ASSERT(lineContents.size() >= 1, "Too few elements in line to be a function")
		FunctionNode& fn = builder->AddFunction(lineContents[0]);

	builder->m_Nodes.pop_back();

	if (lineContents.size() == 1)
	{
		return;
	}

	for (int i = 1; i < lineContents.size(); i++)
	{
		Vector<String> paramParts = SplitStringByChar(lineContents[i], ':');
		GS_ASSERT(paramParts.size() == 2, "Variable definition should have 2 parts, a name and a type hash")
			String name = paramParts[0];
		u64 typeHash = std::stoull(paramParts[1]);
		AddOutputDataSocket(&fn, name, typeHash);
	}

}

void gs::Context::Parser::ParseNode(GraphBuilder* builder, String& line)
{
	// otherwise lookup in node registry
	Node* n = p_Context.GetNode(line);

	if (n)
	{
		builder->m_Nodes.push_back(n->Clone());
		return;
	}
	
	HashString nodeName(line);

	for (auto& [name, func] : builder->m_Functions)
	{
		if (name == nodeName)
		{
			builder->m_Nodes.push_back(func.get());
		}
	}
}

void gs::Context::Parser::ParseVariable(GraphBuilder* builder, String& line)
{
	Vector<String> variableParts = SplitStringByChar(line, ',');
	GS_ASSERT(variableParts.size() == 2, "Variables should contain 2 parts, a name and a type. Too many / few parts to parse variable");

	HashString name = variableParts[0];
	u64 typeHash = std::stoull(variableParts[1]);

	Variable* proto = p_Context.GetVariableFromHash(typeHash)->Clone();

	if (!proto)
	{
		return;
	}

	builder->m_Variables.emplace(name, proto->Clone());
}

void gs::Context::Parser::ParseNodeDataConnection(GraphBuilder* builder, String& line)
{
	Vector<String> both = SplitStringByChar(line, ':');
	GS_ASSERT(both.size() == 2, "Trying to process a connection but there are not exactly 2 parts");

	Vector<String> lhsComponents = SplitStringByChar(both[0], ',');
	Vector<String> rhsComponents = SplitStringByChar(both[1], ',');

	GS_ASSERT(lhsComponents.size() == 3 && rhsComponents.size() == 3, "LHS or RHS of node data connection is not the correct length");

	u64 lhsTypeHash = std::stoull(lhsComponents[2]);
	u64 rhsTypeHash = std::stoull(rhsComponents[2]);

	GS_ASSERT(lhsTypeHash == rhsTypeHash, "Node data connection type is mismatched");

	Node* lhsNode = builder->m_Nodes[std::stoi(lhsComponents[0])];
	Node* rhsNode = builder->m_Nodes[std::stoi(rhsComponents[0])];

	GS_ASSERT(lhsNode != rhsNode, "Cannot connect a node to itself");
	DataConnection* conn = p_Context.GetDataConnectionFromHash(lhsTypeHash)->Clone();

	GS_ASSERT(conn != nullptr, "Failed to find data connection with serialized hash");
	conn = conn->Clone();

	// setup sockets
	DataSocket* lhsSocket = FindNodeDataSocket(lhsNode, lhsComponents[1]);
	DataSocket* rhsSocket = FindNodeDataSocket(rhsNode, rhsComponents[1]);

	conn->m_LHS = lhsSocket;
	conn->m_RHS = rhsSocket;

	builder->m_DataConnections.push_back(conn);
}

void gs::Context::Parser::ParseVariableDataConnection(GraphBuilder* builder, String& line)
{
	Vector<String> parts = SplitStringByChar(line, ':');
	GS_ASSERT(parts.size() == 2, "Should only have 2 parts in a variable data connection");
	Vector<String> lhsVariableParts = SplitStringByChar(parts[0], ',');
	Vector<String> rhsVariableParts = SplitStringByChar(parts[1], ',');

	GS_ASSERT(lhsVariableParts.size() == 2 && rhsVariableParts.size() == 3, "Deserialized variable data connection is not of the correct dimensions");
	DataSocket* lhs = builder->m_Variables[lhsVariableParts[0]]->GetSocket();

	u64 lhsTypeHash = std::stoull(lhsVariableParts[1]);
	u64 rhsTypeHash = std::stoull(rhsVariableParts[2]);

	GS_ASSERT(lhsTypeHash == rhsTypeHash, "Connection between data sockets must be of the same type.")

	Node* rhsNode = builder->m_Nodes[std::stoi(rhsVariableParts[0])];
	HashString rhsSocketName = rhsVariableParts[1];
	DataSocket* rhs = rhsNode->m_InputDataSockets[rhsSocketName];

	DataConnection* conn = p_Context.GetDataConnectionFromHash(rhsTypeHash)->Clone();

	GS_ASSERT(conn, "Could not find prototype data connection from provided hash");

	conn->m_LHS = lhs;
	conn->m_RHS = rhs;

	builder->m_DataConnections.push_back(conn);

}

void gs::Context::Parser::ParseExecutionConnection(GraphBuilder* builder, String& line)
{
	Vector<String> parts = SplitStringByChar(line, ':');
	GS_ASSERT(parts.size() == 2, "Should only have 2 parts in a variable data connection");

	Vector<String> lhsParts = SplitStringByChar(parts[0], ',');
	Vector<String> rhsParts = SplitStringByChar(parts[1], ',');

	GS_ASSERT(lhsParts.size() == 2 && rhsParts.size() == 2, "LHS & RHS of Execution Connection should be composed of 2 parts.");

	int lhsIndex = std::stoi(lhsParts[0]);
	int rhsIndex = std::stoi(rhsParts[0]);

	HashString lhsSocketName = lhsParts[1];
	HashString rhsSocketName = rhsParts[1];


	GS_ASSERT(lhsIndex >= 0 && rhsIndex >= 0, "Could not find node");

	Node* lhsNode = builder->m_Nodes[lhsIndex];
	Node* rhsNode = builder->m_Nodes[rhsIndex];

	ExecutionSocket* lhs = FindNodeExeSocket(lhsNode, lhsSocketName);
	ExecutionSocket* rhs = FindNodeExeSocket(rhsNode, rhsSocketName);


	builder->ConnectExecutionSocket(lhs, rhs);

}

void gs::Context::Parser::ParseDefaultValues(GraphBuilder* builder, String& line)
{
	Vector<String> parts = SplitStringByChar(line, ':');
	GS_ASSERT(parts.size() == 3, "There should be 3 parts in a variable default value, name, hash, val");
	HashString name = parts[0];
	u64 typeHash = std::stoull(parts[1]);
	builder->m_Variables[name]->SetValue(utils::StringToAny(parts[2], typeHash));
}

void gs::Context::Parser::AddOutputDataSocket(Node* node, String name, u64 typeHash)
{
	DataSocket* proto = p_Context.GetSocketFromHash(typeHash);
	if (proto == nullptr)
	{
		return;
	}

	node->m_OutputDataSockets.emplace(name, proto->Clone());
}

gs::DataSocket* gs::Context::Parser::FindNodeDataSocket(Node* node, HashString name)
{
	for (auto& [socketName, socket] : node->m_InputDataSockets)
	{
		if (socketName == name)
		{
			return socket;
		}
	}

	for (auto& [socketName, socket] : node->m_OutputDataSockets)
	{
		if (socketName == name)
		{
			return socket;
		}
	}

	return nullptr;
}

gs::ExecutionSocket* gs::Context::Parser::FindNodeExeSocket(Node* node, HashString name)
{
	for (auto& socket : node->m_InputExecutionSockets)
	{
		if (socket->m_SocketName == name)
		{
			return socket;
		}
	}

	for (auto& socket : node->m_OutputExecutionSockets)
	{
		if (socket->m_SocketName == name)
		{
			return socket;
		}
	}

	return nullptr;
}

gs::Node* gs::Context::GetNode(HashString name)
{
	for (auto& node : p_Nodes)
	{
		if (node->m_NodeName == name)
		{
			return node;
		}
	}
	return nullptr;
}

DataSocket* gs::Context::GetSocketFromHash(u64 typeHash)
{
	for (auto& socket : p_Sockets)
	{
		if (socket->m_Type.m_TypeHash.m_Value == typeHash)
		{
			return socket.get();
		}
	}
	return nullptr;
}

Variable* gs::Context::GetVariableFromHash(u64 typeHash)
{
	for (auto& var : p_Variables)
	{
		if (var->m_Type.m_TypeHash.m_Value == typeHash)
		{
			return var.get();
		}
	}
	return nullptr;
}

DataConnection* gs::Context::GetDataConnectionFromHash(u64 typeHash)
{
	for (auto& conn : p_DataConnections)
	{
		if (conn->m_Type.m_TypeHash.m_Value == typeHash)
		{
			return conn.get();
		}
	}
	return nullptr;
}

gs::Type::Type(const HashString& hash) : m_TypeHash(hash)
{
	int i = 1;
}

void gs::Variable::SetValue(Any a)
{
	m_Value = a;
	GetSocket()->m_Value = a;
}
