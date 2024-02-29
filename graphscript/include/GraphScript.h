//
// Created by Admin on 4/15/2023.
//

#ifndef GRAPHSCRIPT_GUARD_H
#define GRAPHSCRIPT_GUARD_H
#include "TypeDef.h"
namespace gs
{
	using VariableSet = HashMap<HashString, Any>;
	
	enum FunctionCallResult
	{
		Success,
		Fail
	};

	class Type
	{
	public:
		Type(const HashString& hash);
		const HashString m_TypeHash;
	};

	template<typename T>
	class TypeT : public Type
	{
	public:
		TypeT() : Type(GetTypeHash<T>()){}
	};

	template<typename T>
	class GetTypeT
	{
	public:
		inline static const TypeT<T> s_TypeInstance = TypeT<T>();
	};

	class ExecutionSocket
	{
	public:

		ExecutionSocket(HashString socketName, u32 loopCount = 1);

		bool	ShouldExecute()
		{
			return p_ShouldExecute;
		}
		void	SetShouldExecute(bool shouldExecute)
		{
			p_ShouldExecute = shouldExecute;
		}

		u32					m_LoopCount;
		const HashString	m_SocketName;

		ExecutionSocket Clone()
		{
			return {m_SocketName, m_LoopCount};
		}

		ExecutionSocket* CloneHeap()
		{
			return new ExecutionSocket{ m_SocketName, m_LoopCount };
		}
	protected:
		friend class	Graph;
		friend class	GraphBuilder;
		bool			p_ShouldExecute;
		
	};

	class DataSocket
	{
	public:
		Any m_Value;
		const Type& m_Type;

		DataSocket(const Type& type) : m_Type(type){}
		DataSocket(DataSocket& other) = default;
		virtual ~DataSocket();
		virtual DataSocket* Clone() = 0;
	};

	template <typename T>
	class DataSocketT : public DataSocket
	{
	public:
		DataSocketT() : DataSocket(GetTypeT<T>::s_TypeInstance){}

		Optional<T>		Get()
		{
			if (m_Value.has_value())
			{
				return std::any_cast<T>(m_Value);
			}
			return {};
		}
		void Set(const T& other)
		{
			m_Value = other;
		}
		DataSocket* Clone() override
		{
			return new DataSocketT<T>();
		}
	};

	class DataConnection
	{
	public:

		DataConnection(const Type& type) : m_Type(type){}
		virtual void Process() = 0;
		virtual void Print() = 0;
		virtual DataConnection* Clone() = 0;

		DataSocket* m_LHS = nullptr;
		DataSocket* m_RHS = nullptr;

		const Type& m_Type;
	};

	template<typename T>
	class DataConnectionT : public DataConnection
	{
	public:
		DataConnectionT(DataSocketT<T>* lhs, DataSocketT<T>* rhs) : DataConnection(GetTypeT<T>::s_TypeInstance)
		{
			m_LHS = lhs;
			m_RHS = rhs;
		}

		void Process() override
		{
			if (!GetLHS()->Get().has_value())
			{
				return;
			}
			GetRHS()->Set(GetLHS()->Get().value());
		}

		void Print() override
		{
			std::cout << "LHS : " << GetLHS()->Get() << ", RHS : " << GetRHS()->Get() << std::endl;
		}

		DataConnection* Clone() override
		{
			return new DataConnectionT<T>(*this);
		}

		DataSocketT<T>* GetLHS()
		{
			return (DataSocketT<T>*) m_LHS;
		}

		DataSocketT<T>* GetRHS()
		{
			return (DataSocketT<T>*) m_RHS;
		}
		
	protected:
	};

	class Variable
	{
	public:

		Variable(const Type& type) : m_Type(type)
		{

		}

		Any m_Value;
		const Type& m_Type;

		void SetValue(Any a);
		
		virtual Variable*	Clone() = 0;
		virtual DataSocket* GetSocket() = 0;
	};

	template <typename T>
	class VariableT : public Variable
	{
	public:

		VariableT() : Variable(GetTypeT<T>::s_TypeInstance)
		{
		}

		VariableT(T value) : Variable(GetTypeT<T>::s_TypeInstance)
		{
			Set(value);
		}

		VariableT(VariableT<T>& other) = default;

		T		Get()
		{
			T val = std::any_cast<T>(m_Value);
		}
		void	Set(const T& other)
		{
			m_Value = other;
			m_Socket.Set(other);
		}

		Variable*	Clone() override
		{
			return new VariableT<T>(*this);
		}

		DataSocket* GetSocket() override
		{
			return &m_Socket;
		}

		DataSocketT<T> m_Socket;
	};

	class Node
	{
	public:

		Node(HashString nodeName) : m_NodeName(nodeName)
		{
		}
		virtual ~Node();

		virtual void	Process() = 0;
		virtual Node*	Clone() = 0;

		const HashString m_NodeName;

		template <typename T>
		DataSocketT<T>*		AddDataInput(HashString variableName)
		{
			if (m_InputDataSockets.find(variableName) == m_InputDataSockets.end())
			{
				m_InputDataSockets.emplace(variableName, new DataSocketT<T>());
			}
			return static_cast<DataSocketT<T>*>(m_InputDataSockets[variableName]);
		}

		template <typename T>
		DataSocketT<T>*		AddDataOutput(HashString variableName)
		{
			if (m_OutputDataSockets.find(variableName) == m_OutputDataSockets.end())
			{
				m_OutputDataSockets.emplace(variableName, new DataSocketT<T>());
			}
			return static_cast<DataSocketT<T>*>(m_OutputDataSockets[variableName]);
		}

		ExecutionSocket*	AddExecutionInput(HashString name);
		ExecutionSocket*	AddExecutionOutput(HashString name);

		HashMap<HashString, DataSocket*>				m_InputDataSockets;
		HashMap<HashString, DataSocket*>				m_OutputDataSockets;
		Vector<ExecutionSocket*>				m_InputExecutionSockets;
		Vector<ExecutionSocket*>				m_OutputExecutionSockets;
	};

	class FunctionNode : public Node
	{
	public:
		FunctionNode(HashString name);
		FunctionNode(FunctionNode& other) = default;

		template <typename T>
		DataSocketT<T>* AddArgument(HashString variableName)
		{
			if (m_OutputDataSockets.find(variableName) == m_OutputDataSockets.end())
			{
				m_OutputDataSockets.emplace(variableName, new DataSocketT<T>());
			}
			return static_cast<DataSocketT<T>*>(m_OutputDataSockets[variableName]);
		}

		void	Process() override {};

		Node*	Clone() override
		{
			return new FunctionNode(*this);
		}
	};

	class ExecutionConnectionDef
	{
	public:
		ExecutionSocket* m_LHS = nullptr;
		ExecutionSocket* m_RHS = nullptr;
	};
		

	class Graph
	{
	protected:
		HashMap<HashString, FunctionNode*>	p_Functions;
		HashMap<HashString, Variable*>		p_Variables;
		Vector<Node*>						p_Nodes;
		Vector<ExecutionConnectionDef>		p_ExecutionConnections;
		Vector<DataConnection*>				p_DataConnections;

	public:

		Graph(
			HashMap<HashString, FunctionNode*> functions,
			HashMap<HashString, Variable*>	variablesDefs,
			Vector<Node*>						nodes,
			Vector<ExecutionConnectionDef>		executionConnections,
			Vector<DataConnection*>			dataConnections
		);

		~Graph();

		template<typename T>
		void SetVariable(HashString variableName, const T& other)
		{
			if (p_Variables.find(variableName) != p_Variables.end())
			{
				auto varDef = (VariableT<T>*)p_Variables[variableName];
				varDef->Set(other);
			}
		}

		FunctionCallResult CallFunction(HashString nameOfMethod, VariableSet args);
	
protected:
		ExecutionSocket*	FindRHS(ExecutionSocket* lhs);
		Node*				GetNode(ExecutionSocket* socket);
		void				ProcessDataConnections();
		void				PopulateParams(FunctionNode* functionNode, VariableSet params);

		Stack<Node*>	p_Stack;
	};

	class GraphBuilder
	{
	public:
		GraphBuilder();
		~GraphBuilder();

		FunctionNode&				AddFunction(HashString functionName);
		void						AddNode(Node* node);

		template <typename T>
		VariableT<T>* AddVariable(HashString variableName, Optional<T> value = Optional<T>())
		{
			if (m_Variables.find(variableName) == m_Variables.end())
			{
				if (value.has_value())
				{
					m_Variables.emplace(variableName, CreateUnique<VariableT<T>>(value.value()));
				}
				else
				{
					m_Variables.emplace(variableName, CreateUnique<VariableT<T>>());
				}
			}
			return static_cast<VariableT<T>*>(m_Variables[variableName].get());
		}

		template<typename T>
		DataConnectionT<T>*			ConnectDataSocket(DataSocketT<T>* lhs, DataSocketT<T>* rhs)
		{
			auto conn = new DataConnectionT<T>(lhs, rhs);
			m_DataConnections.emplace_back(conn);
			return conn;
		}

		ExecutionConnectionDef		ConnectExecutionSocket(ExecutionSocket* lhs, ExecutionSocket* rhs);

		String						Serialize();


		HashMap<HashString, Unique<Variable>>		m_Variables;
		HashMap<HashString, Unique<FunctionNode>>	m_Functions;
		Vector<Node*>								m_Nodes;
		Vector<DataConnection*>						m_DataConnections;
		Vector<ExecutionConnectionDef>				m_ExecutionConnections;

	protected:
		// Internal Build Methods
		HashMap<HashString, FunctionNode*>	BuildFunctions();
		HashMap<HashString, Variable*>		BuildVariables();
		Vector<Node*>						BuildNodes(HashMap<HashString, FunctionNode*>& functions);
		Vector<ExecutionConnectionDef>		BuildExecutionConnections(HashMap<HashString, FunctionNode*>& functions, Vector<Node*> nodes);
		Vector<DataConnection*>				BuildDataConnections(HashMap<HashString, FunctionNode*>& functions,
			HashMap<HashString, Variable*> variables, Vector<Node*> nodes);

		Node*				FindDataSocketNode(DataSocket* socket);
		Node*				FindExeSocketNode(ExecutionSocket* socket);
		HashString			FindSocketVariableName(DataSocket* socket);
		HashString			FindDataSocketName(DataSocket* socket);
		i32					GetNodeIndex(Node* node);
		void				PrintNodeSockets(Node* node);
		Graph*				Build();
		
		friend class Context;
	};

	class Context
	{
	public:
		Context();

		GraphBuilder*	CreateBuilder();
		GraphBuilder*	DeserializeGraph(String& source);

		Graph*			BuildGraph(GraphBuilder* builder);

		void			AddNode(Node* node);
		Node*			GetNode(HashString name);

		Vector<Node*>& GetAllNodes() { return p_Nodes; }

		template<typename T>
		void RegisterType()
		{
			p_Variables.push_back(CreateUnique<VariableT<T>>());
			p_Sockets.push_back(CreateUnique<DataSocketT<T>>());
			p_DataConnections.push_back(CreateUnique<DataConnectionT<T>>(nullptr, nullptr));
		}

	protected:

		void AddBuiltIns();

		DataSocket*			GetSocketFromHash(u64 typeHash);
		Variable*			GetVariableFromHash(u64 typeHash);
		DataConnection*		GetDataConnectionFromHash(u64 typeHash);

		friend class Parser;
		class Parser
		{
		public:
			Parser(Context& c);

			enum State
			{
				Invalid = 0,
				// BeginFunctions, EndFunctions
				Functions,
				// BeginNodes/EndNodes
				Nodes,
				// BeginVariables/EndVariables
				Variables,
				// BeginNodeDataConns/EndNodeDataConns
				NodeDataConnections,
				// BeginVariableDataConns/EndVariableDataConns
				VariableDataConnections,
				// BeginExeConns/EndExeConns
				ExecutionConnections,
				// BeginDefaultValues/EndDefaultValues
				DefaultVariableValues
			};

			Unique<GraphBuilder> Parse(String& source);

		protected:
			void HandleCurrentState(State& state, String& line);
			void ParseFunction(GraphBuilder* builder, String& line);
			void ParseNode(GraphBuilder* builder, String& line);
			void ParseVariable(GraphBuilder* builder, String& line);
			void ParseNodeDataConnection(GraphBuilder* builder, String& line);
			void ParseVariableDataConnection(GraphBuilder* builder, String& line);
			void ParseExecutionConnection(GraphBuilder* builder, String& line);
			void ParseDefaultValues(GraphBuilder* builder, String& line);

			void				AddOutputDataSocket(Node* node, String name, u64 typeHash);
			DataSocket*			FindNodeDataSocket(Node* node, HashString name);
			ExecutionSocket*	FindNodeExeSocket(Node* node, HashString name);

			Context& p_Context;
			

		};

		// Templates for other types
		Vector<Node*>							p_Nodes;
		Vector<Unique<GraphBuilder>>			p_Builders;
		Vector<Unique<Variable>>				p_Variables;
		Vector<Unique<DataSocket>>				p_Sockets;
		Vector<Unique<DataConnection>>			p_DataConnections;

		// instances
		Vector<Graph*> p_ActiveGraphs;
	};
}
#endif //GRAPHSCRIPT_GUARD_H