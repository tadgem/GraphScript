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
	protected:
		friend class Graph;
		friend class GraphBuilder;
		bool		p_ShouldExecute;
		
	};

	class DataSocket
	{
	public:
		Any m_Value;

		DataSocket() = default;
		DataSocket(DataSocket& other) = default;
		virtual ~DataSocket();
		virtual DataSocket* Clone() = 0;
	};

	template <typename T>
	class DataSocketT : public DataSocket
	{
	public:
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
		virtual void Process() = 0;
		virtual void Print() = 0;
		virtual DataConnection* Clone() = 0;

		DataSocket* m_LHS = nullptr;
		DataSocket* m_RHS = nullptr;
	};

	template<typename T>
	class DataConnectionT : public DataConnection
	{
	public:
		DataConnectionT(DataSocketT<T>* lhs, DataSocketT<T>* rhs)
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
		Any m_Value;
		virtual Variable*	Clone() = 0;
		virtual DataSocket* GetSocket() = 0;
	};

	template <typename T>
	class VariableT : public Variable
	{
	public:

		VariableT() = default;
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
		virtual void	Process() = 0;
		virtual Node*	Clone() = 0;

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
		FunctionNode();
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
		HashMap<HashString, Variable*>		p_VariablesDefs;
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

		template<typename T>
		void SetVariable(HashString variableName, const T& other)
		{
			if (p_VariablesDefs.find(variableName) != p_VariablesDefs.end())
			{
				auto varDef = (VariableT<T>*)p_VariablesDefs[variableName];
				varDef->Set(other);
			}
		}

		FunctionCallResult CallFunction(HashString nameOfMethod, VariableSet args);
	
protected:
		ExecutionSocket*	FindRHS(ExecutionSocket* lhs);
		Node*				GetNode(ExecutionSocket* socket);
		void				ProcessDataConnections();
		void				PopulateParams(FunctionNode* functionNode, VariableSet params);
		void				ResetSockets();

		Stack<Node*>	p_Stack;
	};

	class GraphBuilder
	{
	public:
		~GraphBuilder();

		FunctionNode&				AddFunction(HashString functionName);
		void						AddNode(Node* node);

		template <typename T>
		VariableT<T>*				AddVariable(HashString variableName)
		{
			if (m_VariablesDefs.find(variableName) == m_VariablesDefs.end())
			{
				m_VariablesDefs.emplace(variableName, CreateUnique<VariableT<T>>());
			}
			return static_cast<VariableT<T>*>(m_VariablesDefs[variableName].get());
		}

		template<typename T>
		DataConnectionT<T>*			ConnectDataSocket(DataSocketT<T>* lhs, DataSocketT<T>* rhs)
		{
			auto conn = new DataConnectionT<T>(lhs, rhs);
			m_DataConnections.emplace_back(conn);
			return conn;
		}

		Graph						Build();

		ExecutionConnectionDef		ConnectExecutionSocket(ExecutionSocket* lhs, ExecutionSocket* rhs);

		HashMap<HashString, Unique<Variable>>	m_VariablesDefs;
		HashMap<HashString, Unique<FunctionNode>>	m_Functions;
		Vector<Node*>								m_Nodes;
		Vector<DataConnection*>					m_DataConnections;
		Vector<ExecutionConnectionDef>				m_ExecutionConnections;

	protected:
		// Internal Build Methods
		HashMap<HashString, FunctionNode*>	BuildFunctions();
		HashMap<HashString, Variable*>		BuildVariables();
		Vector<Node*>						BuildNodes(HashMap<HashString, FunctionNode*>& functions);
		Vector<ExecutionConnectionDef>		BuildExecutionConnections(HashMap<HashString, FunctionNode*>& functions, Vector<Node*> nodes);
		Vector<DataConnection*>				BuildDataConnections(HashMap<HashString, FunctionNode*>& functions,
			HashMap<HashString, Variable*> variables, Vector<Node*> nodes);

		Node*				FindSocketNode(DataSocket* socket);
		void				PrintNodeSockets(Node* node);
	};
}
#endif //GRAPHSCRIPT_GUARD_H
