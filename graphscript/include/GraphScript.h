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

	class IExecutionSocket
	{
	public:

		IExecutionSocket(HashString socketName, u32 loopCount = 1);

		bool	ShouldExecute();
		void	SetShouldExecute(bool shouldExecute);

		u32					m_LoopCount;
		const HashString	m_SocketName;

		IExecutionSocket Clone()
		{
			return {m_SocketName, m_LoopCount};
		}
	protected:
		friend class Graph;
		friend class GraphBuilder;
		bool		p_ShouldExecute;
		
	};

	class IDataSocketDef
	{
	public:
		Any m_Value;

		IDataSocketDef() = default;
		IDataSocketDef(IDataSocketDef& other) = default;
		virtual ~IDataSocketDef();
		virtual IDataSocketDef* Clone() = 0;
	};

	template <typename T>
	class IDataSocketDefT : public IDataSocketDef
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
		IDataSocketDef* Clone() override
		{
			return new IDataSocketDefT<T>();
		}
	};

	class IDataConnectionDef
	{
	public:
		virtual void Process() = 0;
		virtual void Print() = 0;
		virtual IDataConnectionDef* Clone() = 0;

		IDataSocketDef* m_LHS = nullptr;
		IDataSocketDef* m_RHS = nullptr;
	};

	template<typename T>
	class IDataConnectionDefT : public IDataConnectionDef
	{
	public:
		IDataConnectionDefT(IDataSocketDefT<T>* lhs, IDataSocketDefT<T>* rhs)
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

		IDataConnectionDef* Clone() override
		{
			return new IDataConnectionDefT<T>(*this);
		}

		IDataSocketDefT<T>* GetLHS()
		{
			return (IDataSocketDefT<T>*) m_LHS;
		}

		IDataSocketDefT<T>* GetRHS()
		{
			return (IDataSocketDefT<T>*) m_RHS;
		}
		
	protected:
	};

	class IVariableDef
	{
	public:
		Any m_Value;
		virtual IVariableDef* Clone() = 0;
		virtual IDataSocketDef* GetSocket() = 0;
	};

	template <typename T>
	class IVariableDefT : public IVariableDef
	{
	public:

		IVariableDefT() = default;
		IVariableDefT(IVariableDefT<T>& other) = default;

		T		Get()
		{
			T val = std::any_cast<T>(m_Value);
		}
		void Set(const T& other)
		{
			m_Value = other;
			m_Socket.Set(other);
		}

		IVariableDef* Clone() override
		{
			return new IVariableDefT<T>(*this);
		}

		IDataSocketDef* GetSocket() override
		{
			return &m_Socket;
		}

		IDataSocketDefT<T> m_Socket;
	};

	class INode
	{
	public:
		virtual void	Process() = 0;
		virtual INode*	Clone() = 0;

		template <typename T>
		IDataSocketDefT<T>* AddDataInput(HashString variableName)
		{
			if (m_InputDataSockets.find(variableName) == m_InputDataSockets.end())
			{
				m_InputDataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_InputDataSockets[variableName]);
		}

		template <typename T>
		IDataSocketDefT<T>* AddDataOutput(HashString variableName)
		{
			if (m_OutputDataSockets.find(variableName) == m_OutputDataSockets.end())
			{
				m_OutputDataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_OutputDataSockets[variableName]);
		}

		IExecutionSocket* AddExecutionInput(HashString name);
		IExecutionSocket* AddExecutionOutput(HashString name);

		HashMap<HashString, IDataSocketDef*>				m_InputDataSockets;
		HashMap<HashString, IDataSocketDef*>				m_OutputDataSockets;
		Vector<IExecutionSocket*>				m_InputExecutionSockets;
		Vector<IExecutionSocket*>				m_OutputExecutionSockets;
	};

	class IFunctionNode : public INode
	{
	public:
		IFunctionNode();
		IFunctionNode(IFunctionNode& other) = default;

		template <typename T>
		IDataSocketDefT<T>* AddArgument(HashString variableName)
		{
			if (m_OutputDataSockets.find(variableName) == m_OutputDataSockets.end())
			{
				m_OutputDataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_OutputDataSockets[variableName]);
		}

		void Process() override {};

		INode* Clone() override
		{
			return new IFunctionNode(*this);
		}
	};

	class IExecutionConnectionDef
	{
	public:
		IExecutionSocket* m_LHS = nullptr;
		IExecutionSocket* m_RHS = nullptr;
	};
		

	class Graph
	{
	protected:
		HashMap<HashString, IFunctionNode*>	p_Functions;
		HashMap<HashString, IVariableDef*>	p_VariablesDefs;
		Vector<INode*>						p_Nodes;
		Vector<IExecutionConnectionDef>		p_ExecutionConnections;
		Vector<IDataConnectionDef*>			p_DataConnections;
		
	public:

		Graph(
			HashMap<HashString, IFunctionNode*> functions,
			HashMap<HashString, IVariableDef*>	variablesDefs,
			Vector<INode*>						nodes,
			Vector<IExecutionConnectionDef>		executionConnections,
			Vector<IDataConnectionDef*>			dataConnections
		);

		template<typename T>
		void SetVariable(HashString variableName, const T& other)
		{
			if (p_VariablesDefs.find(variableName) != p_VariablesDefs.end())
			{
				auto varDef = (IVariableDefT<T>*)p_VariablesDefs[variableName];
				varDef->Set(other);
			}
		}

		FunctionCallResult CallFunction(HashString nameOfMethod, VariableSet args);
	
protected:
		IExecutionSocket*	FindRHS(IExecutionSocket* lhs);
		INode*				GetNode(IExecutionSocket* socket);
		void				ProcessDataConnections();
		void				PopulateParams(IFunctionNode* functionNode, VariableSet params);
		void				ResetSockets();

		Stack<INode*>	p_Stack;
	};

	class GraphBuilder
	{
	public:
		~GraphBuilder();

		IFunctionNode&				AddFunction(HashString functionName);
		void						AddNode(INode* node);

		template <typename T>
		IVariableDefT<T>*			AddVariable(HashString variableName)
		{
			if (m_VariablesDefs.find(variableName) == m_VariablesDefs.end())
			{
				m_VariablesDefs.emplace(variableName, CreateUnique<IVariableDefT<T>>());
			}
			return static_cast<IVariableDefT<T>*>(m_VariablesDefs[variableName].get());
		}

		template<typename T>
		IDataConnectionDefT<T>*		ConnectDataSocket(IDataSocketDefT<T>* lhs, IDataSocketDefT<T>* rhs)
		{
			auto conn = new IDataConnectionDefT<T>(lhs, rhs);
			m_DataConnections.emplace_back(conn);
			return conn;
		}

		Graph						Build();

		IExecutionConnectionDef	ConnectExecutionSocket(IExecutionSocket* lhs, IExecutionSocket* rhs);

		HashMap<HashString, Unique<IVariableDef>>	m_VariablesDefs;
		HashMap<HashString, Unique<IFunctionNode>>	m_Functions;
		Vector<INode*>								m_Nodes;
		Vector<IDataConnectionDef*>					m_DataConnections;
		Vector<IExecutionConnectionDef>				m_ExecutionConnections;

	protected:
		// Internal Build Methods
		HashMap<HashString, IFunctionNode*> BuildFunctions();
		HashMap<HashString, IVariableDef*>	BuildVariablesDefs();
		Vector<INode*>						BuildNodes(HashMap<HashString, IFunctionNode*>& functions);
		Vector<IExecutionConnectionDef>		BuildExecutionConnections(HashMap<HashString, IFunctionNode*>& functions);
		Vector<IDataConnectionDef*>			BuildDataConnections(HashMap<HashString, IFunctionNode*>& functions,
			HashMap<HashString, IVariableDef*> variables);

		INode*				FindSocketNode(IDataSocketDef* socket);
		void				PrintNodeSockets(INode* node);
	};
}
#endif //GRAPHSCRIPT_GUARD_H
