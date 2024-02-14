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

		IExecutionSocket(u32 loopCount = 0);

		bool ShouldExecute();
		void Execute();

	protected:
		bool		p_ShouldExecute;
		const u32	p_LoopCount;
	};

	class IDataSocketDef
	{
	public:
		Any m_Value;

		IDataSocketDef() = default;
		IDataSocketDef(IDataSocketDef& other) = default;
		virtual ~IDataSocketDef();
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
	};

	class IDataConnectionDef
	{
	public:
		virtual void Process() = 0;
		virtual void Print() = 0;
		virtual IDataConnectionDef* Clone() = 0;

		IDataSocketDef* m_LHS;
		IDataSocketDef* m_RHS;
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
		virtual void Process() = 0;

		HashMap<HashString, IDataSocketDef*> m_InputDataSockets;
		HashMap<HashString, IDataSocketDef*> m_OutputDataSockets;
	};

	class ICustomNode : public INode
	{
	public:
		template <typename T>
		IDataSocketDefT<T>* AddInput(HashString variableName)
		{
			if (m_InputDataSockets.find(variableName) == m_InputDataSockets.end())
			{
				m_InputDataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_InputDataSockets[variableName]);
		}

		template <typename T>
		IDataSocketDefT<T>* AddOutput(HashString variableName)
		{
			if (m_OutputDataSockets.find(variableName) == m_OutputDataSockets.end())
			{
				m_OutputDataSockets.emplace(variableName, new IDataSocketDefT<T>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_OutputDataSockets[variableName]);
		}

		void AddFunctionality(Procedure proc)
		{
			m_Proc = proc;
		}

		void Process() override;


		Procedure m_Proc = NULL;
	};

	class IFunctionNode : public INode
	{
	public:
		IFunctionNode() = default;
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

		IFunctionNode* Clone()
		{
			return new IFunctionNode(*this);
		}
	};

	class IExecutionConnectionDef
	{
	public:
		INode* m_LHS = nullptr;
		INode* m_RHS = nullptr;
	};

	class IExecutionConnectionDefV2
	{
	public:
		IExecutionSocket* m_LHS = nullptr;
		IExecutionSocket* m_RHS = nullptr;
	};
		

	class Graph
	{
	protected:
		HashMap<HashString, IFunctionNode*> m_Functions;
		HashMap<HashString, IVariableDef*> m_VariablesDefs;
		Vector<INode*> m_Nodes;
		Vector<IExecutionConnectionDef> m_ExecutionConnections;
		Vector<IDataConnectionDef*> m_DataConnections;
	public:

		Graph(
			HashMap<HashString, IFunctionNode*> functions,
			HashMap<HashString, IVariableDef*> variablesDefs,
			Vector<INode*> nodes,
			Vector<IExecutionConnectionDef> executionConnections,
			Vector<IDataConnectionDef*> dataConnections
		);

		template<typename T>
		void SetVariable(HashString variableName, const T& other)
		{
			if (m_VariablesDefs.find(variableName) != m_VariablesDefs.end())
			{
				IVariableDefT<T>* varDef = (IVariableDefT<T>*)m_VariablesDefs[variableName];
				varDef->Set(other);
			}
		}

		FunctionCallResult CallFunction(HashString nameOfMethod, VariableSet args);
	
protected:
		INode*	FindRHS(INode* lhs);
		void	ProcessDataConnections();
		void	PopulateParams(IFunctionNode* functionNode, VariableSet params);
		void	ResetSockets();
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
		IDataConnectionDefT<T>*		ConnectSocket(IDataSocketDefT<T>* lhs, IDataSocketDefT<T>* rhs)
		{
			auto conn = new IDataConnectionDefT<T>(lhs, rhs);
			m_DataConnections.emplace_back(conn);
			return conn;
		}

		Graph						Build();

		IExecutionConnectionDef		ConnectNode(INode* lhs, INode* rhs);

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

		INode*	FindSocketNode(IDataSocketDef* socket);
		void	PrintNodeSockets(INode* node);
	};
}
#endif //GRAPHSCRIPT_GUARD_H
