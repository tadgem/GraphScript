#include "App.h"
#include <imgui.h>
#include "imnodes.h"
#include "GraphScript.h"
#include "BuiltInNodes.h"

const int ITERATIONS = 3;
int main() {
	gs::ExampleApp app;

	if (!app.Init())
	{
		return -1;
	}

	const int hardcoded_node_id = 1;

	gs::GraphBuilder builder;
	// create a graph variable
	gs::IVariableDefT<float>* var = builder.AddVariable<float>("NameOfVariable");
	gs::IVariableDefT<bool>* boolVar = builder.AddVariable<bool>("ConditionVariable");
	
	// Create a function entry point
	gs::IFunctionNode& entry = builder.AddFunction("NameOfEntry");
	// get a reference to the output execution socket, which is created by default for function nodes
	gs::IExecutionSocket* entryExecutionSocket = entry.m_OutputExecutionSockets.front();
	// add an argument to the function
	gs::IDataSocketDefT<float>* param1 = entry.AddArgument<float>("NameOfParameter");
	
	// create a multiply float node 
	auto multiplyNodeBuilder = gs::CreateUnique<gs::MutliplyNodeT<float>>();

	// IF node
	auto ifNodeBuilder = gs::CreateUnique<gs::ICustomNode>();
	gs::IExecutionSocket* ifInExecutionSocket = ifNodeBuilder->AddExecutionInput("in");
	gs::IExecutionSocket* ifTrueExecutionSocket = ifNodeBuilder->AddExecutionOutput("true");
	gs::IExecutionSocket* ifFalseExecutionSocket = ifNodeBuilder->AddExecutionOutput("false");
	// add an input parameter for the condition
	gs::IDataSocketDefT<bool>* ifInputParam = ifNodeBuilder->AddDataInput<bool>("condition");
	ifNodeBuilder->AddFunctionality([&ifInputParam, &ifTrueExecutionSocket, &ifFalseExecutionSocket]()
	{
			if (!ifInputParam->Get().has_value())
			{
				return;
			}
			bool condition = ifInputParam->Get().value();
			if (condition)
			{
				ifTrueExecutionSocket->SetShouldExecute(true);
				ifFalseExecutionSocket->SetShouldExecute(false);
			}
			else
			{
				ifTrueExecutionSocket->SetShouldExecute(false);
				ifFalseExecutionSocket->SetShouldExecute(true);
			}
	});

	// need to rethink this a bit..
	// probably need to clone nodes too
	// we have nowhere to store the current index in the node 
	// need to be able to store the count in the socket per for loop
	// with this we can then set the execution socket based on the loop count
	// and also provide the 
	// FOR node
	auto forNodeBuilder = gs::CreateUnique<gs::ICustomNode>();
	gs::IExecutionSocket*			forInExecutionSocket	= forNodeBuilder->AddExecutionInput("in");
	gs::IExecutionSocket*			forOutExecutionSocket	= forNodeBuilder->AddExecutionOutput("out");
	gs::IExecutionSocket*			forLoopExecutionSocket	= forNodeBuilder->AddExecutionOutput("each");
	gs::IDataSocketDefT<gs::u32>*	forCountInputSocket		= forNodeBuilder->AddDataInput<gs::u32>("count");
	forNodeBuilder->AddFunctionality([&forOutExecutionSocket, &forLoopExecutionSocket, &forCountInputSocket]()
		{
			if (!forCountInputSocket->Get().has_value())
			{
				return;
			}
			forOutExecutionSocket->SetShouldExecute(false);
			forLoopExecutionSocket->SetShouldExecute(true);
		});

	// print float node
	auto printFloatNodeBuilder = gs::CreateUnique<gs::ICustomNode>();
	gs::IExecutionSocket* printInputExecution = printFloatNodeBuilder->AddExecutionInput("in");
	gs::IExecutionSocket* printOutputExecution = printFloatNodeBuilder->AddExecutionInput("out");
	gs::IDataSocketDefT<float>* floatInputParam = multiplyNodeBuilder->AddDataInput<float>("input");
	printFloatNodeBuilder->AddFunctionality([&floatInputParam]()
		{
			std::cout << floatInputParam->Get().value() << std::endl;
		});

	// make sure the builder knows about the nodes
	builder.AddNode(multiplyNodeBuilder.get());
	builder.AddNode(ifNodeBuilder.get());
	builder.AddNode(printFloatNodeBuilder.get());

	// connect the function parameter socket to the input socket of the multiply node
	gs::IDataConnectionDefT<float>* entryToInputDef = builder.ConnectDataSocket<float>(param1, (IDataSocketDefT<float>*) multiplyNodeBuilder->m_InputDataSockets["input"]);
	// connect the graph variable socket to the multiple socket of the multiply node
	gs::IDataConnectionDefT<float>* varToMultipleDef = builder.ConnectDataSocket<float>(&var->m_Socket, (IDataSocketDefT<float>*) multiplyNodeBuilder->m_InputDataSockets["multiple"]);
	// connect the result socket of the multiply node to the input socket of the print float node
	gs::IDataConnectionDefT<float>* outputToPrintDef = builder.ConnectDataSocket<float>((IDataSocketDefT<float>*) multiplyNodeBuilder->m_OutputDataSockets["result"], floatInputParam);
	// connect bool variable to the if node
	gs::IDataConnectionDefT<bool>* boolToIfDef = builder.ConnectDataSocket<bool>(&boolVar->m_Socket, ifInputParam);

	// connect execution sockets from function entry to mutiply in
	gs::IExecutionConnectionDef entryToMultiplyExecution = builder.ConnectExecutionSocket(entryExecutionSocket, multiplyNodeBuilder->m_InputExecutionSockets.front());
	// connect multiply out to if in
	gs::IExecutionConnectionDef multiplyToIfExecution = builder.ConnectExecutionSocket(multiplyNodeBuilder->m_OutputExecutionSockets.front(), ifInExecutionSocket);
	// connect true socket to print in
	gs::IExecutionConnectionDef trueToPrintExecution = builder.ConnectExecutionSocket(ifTrueExecutionSocket, printInputExecution);
	
	// Build the graph to an executable object
	gs::Graph g1 = builder.Build();
	// cache the function name we want to call
	gs::HashString entryName("NameOfEntry");

	// set the multiple variable for this graph instance
	g1.SetVariable<float>("NameOfVariable", 3.0f);
	g1.SetVariable<bool>("ConditionVariable", true);
	// create an argument set to pass the function
	gs::VariableSet args;
	// create a named argument (must match the name of the function parameter socket)
	args["NameOfParameter"] = 3.0f;

	for (int i = 0; i < ITERATIONS; i++)
	{
		g1.CallFunction(entryName, args);
	}

	TIMER(GS_Timer,
	{
		for (int i = 0; i < ITERATIONS; i++)
		{
			g1.CallFunction(entryName, args);
		}
	})

	TIMER(C_Timer,
	{
		for (int i = 0; i < ITERATIONS; i++)
		{
			float x = 3.0 * 3.0;
			std::cout << "C : " << x << std::endl;
		}
	})

	while (app.ShouldRun())
	{
		if(ImGui::Begin("Hello"))
		{
			ImNodes::BeginNodeEditor();

			ImNodes::BeginNode(hardcoded_node_id);
			ImGui::Dummy(ImVec2(80.0f, 45.0f));
			ImNodes::EndNode();

			ImNodes::EndNodeEditor();
		}
		ImGui::End();

		app.PostFrame();
	}

}
