#include "App.h"
#include <imgui.h>
#include "imnodes.h"
#include "GraphScript.h"

const int ITERATIONS = 100000;
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
	
	// Create a function entry point
	gs::IFunctionNode& entry = builder.AddFunction("NameOfEntry");
	// get a reference to the output execution socket, which is created by default for function nodes
	gs::IExecutionSocket* entryExecutionSocket = entry.m_OutputExecutionSockets["out"];
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

	// Same as above
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
	builder.AddNode(printFloatNodeBuilder.get());

	// connect the function parameter socket to the input socket of the multiply node
	gs::IDataConnectionDefT<float>* entryToInputDef = builder.ConnectDataSocket<float>(param1, inputParam);
	// connect the graph variable socket to the multiple socket of the multiply node
	gs::IDataConnectionDefT<float>* varToMultipleDef = builder.ConnectDataSocket<float>(&var->m_Socket, multipleParam);
	// connect the result socket of the multiply node to the input socket of the print float node
	gs::IDataConnectionDefT<float>* outputToPrintDef = builder.ConnectDataSocket<float>(resultDef, floatInputParam);
	
	// connect execution sockets from function entry to mutiply in
	gs::IExecutionConnectionDef entryToMultiplyExecution = builder.ConnectExecutionSocket(entryExecutionSocket, multiplyInputExecution);
	// connect execution sockets from multiply out to print in
	gs::IExecutionConnectionDef multiplyToPrintExecution = builder.ConnectExecutionSocket(multiplyOutputExecution, printInputExecution);
	
	// Build the graph to an executable object
	gs::Graph g1 = builder.Build();
	// cache the function name we want to call
	gs::HashString entryName("NameOfEntry");

	// set the multiple variable for this graph instance
	g1.SetVariable<float>("NameOfVariable", 3.0f);
	// create an argument set to pass the function
	gs::VariableSet args;
	// create a named argument (must match the name of the function parameter socket)
	args["NameOfParameter"] = 3.0f;

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
