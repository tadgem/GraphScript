#include "App.h"
#include <imgui.h>
#include "imnodes.h"
#include "GraphScript.h"
#include "BuiltInNodes.h"

const int ITERATIONS = 2;
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
	gs::IVariableDefT<u32>* countVar = builder.AddVariable<u32>("LoopCount");

	// Create a function entry point
	gs::IFunctionNode& entry = builder.AddFunction("NameOfEntry");
	// get a reference to the output execution socket, which is created by default for function nodes
	gs::IExecutionSocket* entryExecutionSocket = entry.m_OutputExecutionSockets.front();
	// add an argument to the function
	gs::IDataSocketDefT<float>* param1 = entry.AddArgument<float>("NameOfParameter");
	
	// for node 
	auto forNodeBuilder = gs::CreateUnique<gs::ForNode>();
	// create a multiply float node 
	auto multiplyNodeBuilder = gs::CreateUnique<gs::MutliplyNodeT<float>>();
	// IF node
	auto ifNodeBuilder = gs::CreateUnique<gs::IfNode>();
	// print float node
	auto printFloatNodeBuilder = gs::CreateUnique<gs::PrintNodeT<float>>();


	// make sure the builder knows about the nodes
	builder.AddNode(forNodeBuilder.get());
	builder.AddNode(multiplyNodeBuilder.get());
	builder.AddNode(ifNodeBuilder.get());
	builder.AddNode(printFloatNodeBuilder.get());

	// connect the function parameter socket to the input socket of the multiply node
	gs::IDataConnectionDefT<float>* entryToInputDef = builder.ConnectDataSocket<float>(param1, (IDataSocketDefT<float>*) multiplyNodeBuilder->m_InputDataSockets["input"]);
	// connect int count to loop count
	gs::IDataConnectionDefT<u32>* loopCountConn = builder.ConnectDataSocket<u32>(&countVar->m_Socket, (IDataSocketDefT<u32>*)forNodeBuilder->m_InputDataSockets["count"]);
	// connect the graph variable socket to the multiple socket of the multiply node
	gs::IDataConnectionDefT<float>* varToMultipleDef = builder.ConnectDataSocket<float>(&var->m_Socket, (IDataSocketDefT<float>*) multiplyNodeBuilder->m_InputDataSockets["multiple"]);
	// connect the result socket of the multiply node to the input socket of the print float node
	gs::IDataConnectionDefT<float>* outputToPrintDef = builder.ConnectDataSocket<float>((IDataSocketDefT<float>*) multiplyNodeBuilder->m_OutputDataSockets["result"], (IDataSocketDefT<float>*)printFloatNodeBuilder->m_InputDataSockets["input"]);
	// connect bool variable to the if node
	gs::IDataConnectionDefT<bool>* boolToIfDef = builder.ConnectDataSocket<bool>(&boolVar->m_Socket, (IDataSocketDefT<bool>*) ifNodeBuilder->m_InputDataSockets["condition"]);

	// connect execution sockets from function entry to mutiply in
	gs::IExecutionConnectionDef entryToForExecution = builder.ConnectExecutionSocket(entryExecutionSocket, forNodeBuilder->m_InputExecutionSockets.front());
	// connect for out to multiply in
	gs::IExecutionConnectionDef forToMultiplyExecution = builder.ConnectExecutionSocket(forNodeBuilder->m_OutputExecutionSockets[1], multiplyNodeBuilder->m_InputExecutionSockets.front());
	// connect multiply out to if in
	gs::IExecutionConnectionDef multiplyToIfExecution = builder.ConnectExecutionSocket(multiplyNodeBuilder->m_OutputExecutionSockets.front(), ifNodeBuilder->m_InputExecutionSockets.front());
	// connect true socket to print in
	gs::IExecutionConnectionDef trueToPrintExecution = builder.ConnectExecutionSocket(ifNodeBuilder->m_OutputExecutionSockets.front(), printFloatNodeBuilder->m_InputExecutionSockets.front());
	

	// Build the graph to an executable object
	gs::Graph g1 = builder.Build();
	// cache the function name we want to call
	gs::HashString entryName("NameOfEntry");


	// set the multiple variable for this graph instance
	g1.SetVariable<float>("NameOfVariable", 3.0f);
	g1.SetVariable<bool>("ConditionVariable", true);
	g1.SetVariable<u32>("LoopCount", 5);
	// create an argument set to pass the function
	gs::VariableSet args;
	// create a named argument (must match the name of the function parameter socket)
	args["NameOfParameter"] = 3.0f;

	for (int i = 0; i < ITERATIONS; i++)
	{
		g1.CallFunction(entryName, args);
	}
	/*
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
	*/
}
