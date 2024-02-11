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

	class IDataSocketDef
	{
	public:
		Any m_Value;
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
	};

	template<typename T>
	class IDataConnectionDefT : public IDataConnectionDef
	{
	public:
		IDataConnectionDefT(IDataSocketDefT<T>* lhs, IDataSocketDefT<T>* rhs)
		{
			p_LHS = lhs;
			p_RHS = rhs;
		}

		void Process() override
		{
			if (!p_LHS->Get().has_value())
			{
				return;
			}
			p_RHS->Set(p_LHS->Get().value());
		}

		void Print() override
		{
			std::cout << "LHS : " << p_LHS->Get() << ", RHS : " << p_RHS->Get() << std::endl;
			//std::cout << "Hello" << std::endl;
		}

	protected:
		IDataSocketDefT<T>* p_LHS;
		IDataSocketDefT<T>* p_RHS;
	};

	class IVariableDef
	{
	public:
		Any m_Value;
	};

	template <typename T>
	class IVariableDefT : public IVariableDef
	{
	public:
		T		Get()
		{
			T val = std::any_cast<T>(m_Value);
		}
		void Set(const T& other)
		{
			m_Value = other;
			m_Socket.Set(other);
		}

		IDataSocketDefT<T> m_Socket;
	};

	class INode
	{
	public:
		virtual void Process() = 0;

		HashMap<HashString, Unique<IDataSocketDef>> m_InputDataSockets;
		HashMap<HashString, Unique<IDataSocketDef>> m_OutputDataSockets;
	};

	class ICustomNode : public INode
	{
	public:
		template <typename T>
		IDataSocketDefT<T>* AddInput(HashString variableName)
		{
			if (m_InputDataSockets.find(variableName) == m_InputDataSockets.end())
			{
				m_InputDataSockets.emplace(variableName, CreateUnique<IDataSocketDefT<T>>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_InputDataSockets[variableName].get());
		}

		template <typename T>
		IDataSocketDefT<T>* AddOutput(HashString variableName)
		{
			if (m_OutputDataSockets.find(variableName) == m_OutputDataSockets.end())
			{
				m_OutputDataSockets.emplace(variableName, CreateUnique<IDataSocketDefT<T>>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_OutputDataSockets[variableName].get());
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
		template <typename T>
		IDataSocketDefT<T>* AddArgument(HashString variableName)
		{
			if (m_OutputDataSockets.find(variableName) == m_OutputDataSockets.end())
			{
				m_OutputDataSockets.emplace(variableName, CreateUnique<IDataSocketDefT<T>>());
			}
			return static_cast<IDataSocketDefT<T>*>(m_OutputDataSockets[variableName].get());
		}

		void Process() override {};
	};

	class IExecutionConnectionDef
	{
	public:
		INode* m_LHS;
		INode* m_RHS;
	};

	class Graph
	{
	public:
	};

	class GraphBuilder
	{
	public:

		~GraphBuilder();

		IFunctionNode& AddFunction(HashString functionName);
		void AddNode(INode* node);

		template <typename T>
		IVariableDefT<T>* AddVariable(HashString variableName)
		{
			if (m_VariablesDefs.find(variableName) == m_VariablesDefs.end())
			{
				m_VariablesDefs.emplace(variableName, CreateUnique<IVariableDefT<T>>());
			}
			return static_cast<IVariableDefT<T>*>(m_VariablesDefs[variableName].get());
		}

		template<typename T>
		IDataConnectionDefT<T>* ConnectSocket(IDataSocketDefT<T>* lhs, IDataSocketDefT<T>* rhs)
		{
			auto conn = new IDataConnectionDefT<T>(lhs, rhs);
			m_DataConnections.emplace_back(conn);
			return conn;
		}

		template<typename T>
		void SetVariable(HashString variableName, const T& other)
		{
			if (m_VariablesDefs.find(variableName) != m_VariablesDefs.end())
			{
				IVariableDefT<T>* varDef = (IVariableDefT<T>*)m_VariablesDefs[variableName].get();
				varDef->Set(other);
			}
		}

		IExecutionConnectionDef ConnectNode(INode* lhs, INode* rhs);
		FunctionCallResult CallFunction(HashString nameOfMethod, VariableSet args);

		HashMap<HashString, Unique<IVariableDef>> m_VariablesDefs;
		HashMap<HashString, IFunctionNode> m_Functions;
		Vector<INode*> m_Nodes;
		Vector<IDataConnectionDef*> m_DataConnections;
		Vector<IExecutionConnectionDef> m_ExecutionConnections;

	protected:
		VariableSet p_VariableData;
		INode* FindRHS(INode* lhs);
		INode* FindSocketNode(IDataSocketDef* socket);
		void PrintNodeSockets(INode* node);
		void ProcessDataConnections();
		void PopulateParams(IFunctionNode& functionNode, VariableSet params);
		void ResetSockets();
	};
}
#endif //GRAPHSCRIPT_GUARD_H
