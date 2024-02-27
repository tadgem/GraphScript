#include "App.h"
#include <imgui.h>
#include "imnodes.h"
#include "GraphScript.h"
#include "GraphScriptEditor.h"
#include "BuiltInNodes.h"

const int ITERATIONS = 2;
int main() {
	using namespace gs;
	ExampleApp app("Editor");
	Context context;

	context.RegisterType<float>();
	context.RegisterType<u32>();

	GraphBuilder& builder = *context.CreateBuilder();
	// create a graph variable
	VariableT<float>* var		= builder.AddVariable<float>("NameOfVariable");
	VariableT<bool>* boolVar	= builder.AddVariable<bool>("ConditionVariable");
	VariableT<u32>* countVar	= builder.AddVariable<u32>("LoopCount", 10);

	// Create a function entry point
	FunctionNode& entry = builder.AddFunction("NameOfEntry");
	// get a reference to the output execution socket, which is created by default for function nodes
	ExecutionSocket* entryExecutionSocket = entry.m_OutputExecutionSockets.front();
	// add an argument to the function
	DataSocketT<float>* param1 = entry.AddArgument<float>("NameOfParameter");

	// for node 
	auto forNodeBuilder = CreateUnique<ForNode>();
	// create a multiply float node 
	auto multiplyNodeBuilder = CreateUnique<MutliplyNodeT<float>>();
	// IF node
	auto ifNodeBuilder = CreateUnique<IfNode>();
	// print float node
	auto printFloatNodeBuilder = CreateUnique<PrintNodeT<float>>();


	// make sure the builder knows about the nodes
	builder.AddNode(forNodeBuilder.get());
	builder.AddNode(multiplyNodeBuilder.get());
	builder.AddNode(ifNodeBuilder.get());
	builder.AddNode(printFloatNodeBuilder.get());

	// connect the function parameter socket to the input socket of the multiply node
	DataConnectionT<float>* entryToInputDef = builder.ConnectDataSocket<float>(param1, (DataSocketT<float>*) multiplyNodeBuilder->m_InputDataSockets["input"]);
	// connect int count to loop count
	DataConnectionT<u32>* loopCountConn = builder.ConnectDataSocket<u32>(&countVar->m_Socket, (DataSocketT<u32>*)forNodeBuilder->m_InputDataSockets["count"]);
	// connect the graph variable socket to the multiple socket of the multiply node
	DataConnectionT<float>* varToMultipleDef = builder.ConnectDataSocket<float>(&var->m_Socket, (DataSocketT<float>*) multiplyNodeBuilder->m_InputDataSockets["multiple"]);
	// connect the result socket of the multiply node to the input socket of the print float node
	DataConnectionT<float>* outputToPrintDef = builder.ConnectDataSocket<float>((DataSocketT<float>*) multiplyNodeBuilder->m_OutputDataSockets["result"], (DataSocketT<float>*)printFloatNodeBuilder->m_InputDataSockets["input"]);
	// connect bool variable to the if node
	DataConnectionT<bool>* boolToIfDef = builder.ConnectDataSocket<bool>(&boolVar->m_Socket, (DataSocketT<bool>*) ifNodeBuilder->m_InputDataSockets["condition"]);

	// connect execution sockets from function entry to mutiply in
	ExecutionConnectionDef entryToForExecution = builder.ConnectExecutionSocket(entryExecutionSocket, forNodeBuilder->m_InputExecutionSockets.front());
	// connect for out to multiply in
	ExecutionConnectionDef forToMultiplyExecution = builder.ConnectExecutionSocket(forNodeBuilder->m_OutputExecutionSockets[1], multiplyNodeBuilder->m_InputExecutionSockets.front());
	// connect multiply out to if in
	ExecutionConnectionDef multiplyToIfExecution = builder.ConnectExecutionSocket(multiplyNodeBuilder->m_OutputExecutionSockets.front(), ifNodeBuilder->m_InputExecutionSockets.front());
	// connect true socket to print in
	ExecutionConnectionDef trueToPrintExecution = builder.ConnectExecutionSocket(ifNodeBuilder->m_OutputExecutionSockets.front(), printFloatNodeBuilder->m_InputExecutionSockets.front());


	// Build the graph to an executable object
	// need to rework build function completely
	// to map cloned nodes and sockets to their builder equivalents
	// types shouldnt be an issue as the clone method will handle type deduction
	// we just need to set new raw lhs / rhs pointers.
	Graph& g1 = *context.BuildGraph(&builder);
	Graph& g2 = *context.BuildGraph(&builder);

	// save builder to string
	String graphString = builder.Serialize();
	FStream outFile = FStream("output.gs");
	outFile << graphString;
	outFile.close();
	// cache the function name we want to call
	HashString entryName("NameOfEntry");


	// set the multiple variable for this graph instance
	g1.SetVariable<float>("NameOfVariable", 3.0f);
	g1.SetVariable<bool>("ConditionVariable", true);
	//g1.SetVariable<u32>("LoopCount", 2);

	g2.SetVariable<float>("NameOfVariable", 4.0f);
	g2.SetVariable<bool>("ConditionVariable", true);
	//g2.SetVariable<u32>("LoopCount", 3);
	// create an argument set to pass the function
	VariableSet args;
	// create a named argument (must match the name of the function parameter socket)
	args["NameOfParameter"] = 3.0f;

	for (int i = 0; i < ITERATIONS; i++)
	{
		g1.CallFunction(entryName, args);
		g2.CallFunction(entryName, args);
	}

	GraphScriptEditor editor(&context, &builder);

	while (app.ShouldRun())
	{
		if(ImGui::Begin("Hello"))
		{
			editor.OnImGui();
		}
		ImGui::End();

		app.PostFrame();
	}
}
