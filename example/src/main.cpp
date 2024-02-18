#include "App.h"
#include <imgui.h>
#include "imnodes.h"
#include "GraphScript.h"

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
	auto multiplyNodeBuilder = gs::CreateUnique<gs::ICustomNode>();
	// in / out execution
	gs::IExecutionSocket* multiplyInputExecution		= multiplyNodeBuilder->AddExecutionInput("in");
	gs::IExecutionSocket* multiplyOutputExecution		= multiplyNodeBuilder->AddExecutionOutput("out");
	// add an input parameter for the number to be multiplied
	gs::IDataSocketDefT<float>* inputParam		= multiplyNodeBuilder->AddDataInput<float>("input");
	// add an input parameter for the number to multiply by
	gs::IDataSocketDefT<float>* multipleParam	= multiplyNodeBuilder->AddDataInput<float>("multiple");
	// add an output parameter for the result
	gs::IDataSocketDefT<float>* resultDef		= multiplyNodeBuilder->AddDataOutput<float>("result");
	// define implementation
	multiplyNodeBuilder->AddFunctionality([ & inputParam, &multipleParam, &resultDef]()
	{
			if (!inputParam->Get().has_value() || !multipleParam->Get().has_value())
			{
				return;
			}
			resultDef->Set(inputParam->Get().value() * multipleParam->Get().value());
	 
	});

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

	// need to change this up
	// the socket needs to be fed a loop count from the node
	// negating need to keep count on the stack, just need the socket
	// also gives us a way to get index in the loop e.g. i = Count - (Socket.Count)

	// FOR node
	auto forNodeBuilder = gs::CreateUnique<gs::ICustomNode>();
	gs::IExecutionSocket* forInExecutionSocket = forNodeBuilder->AddExecutionInput("in");
	gs::IExecutionSocket* forOutExecutionSocket = forNodeBuilder->AddExecutionOutput("out");
	gs::IExecutionSocket* forLoopExecutionSocket = forNodeBuilder->AddExecutionOutput("each");
	gs::IDataSocketDefT<gs::u32>* forCountInputSocket = forNodeBuilder->AddDataInput<gs::u32>("count");


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
	gs::IDataConnectionDefT<float>* entryToInputDef = builder.ConnectDataSocket<float>(param1, inputParam);
	// connect the graph variable socket to the multiple socket of the multiply node
	gs::IDataConnectionDefT<float>* varToMultipleDef = builder.ConnectDataSocket<float>(&var->m_Socket, multipleParam);
	// connect the result socket of the multiply node to the input socket of the print float node
	gs::IDataConnectionDefT<float>* outputToPrintDef = builder.ConnectDataSocket<float>(resultDef, floatInputParam);
	// connect bool variable to the if node
	gs::IDataConnectionDefT<bool>* boolToIfDef = builder.ConnectDataSocket<bool>(&boolVar->m_Socket, ifInputParam);

	// connect execution sockets from function entry to mutiply in
	gs::IExecutionConnectionDef entryToMultiplyExecution = builder.ConnectExecutionSocket(entryExecutionSocket, multiplyInputExecution);
	// connect multiply out to if in
	gs::IExecutionConnectionDef multiplyToIfExecution = builder.ConnectExecutionSocket(multiplyOutputExecution, ifInExecutionSocket);
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
