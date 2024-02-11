#include "App.h"
#include <imgui.h>
#include "imnodes.h"
#include "GraphScript.h"


int main() {
	gs::ExampleApp app;

	if (!app.Init())
	{
		return -1;
	}

	const int hardcoded_node_id = 1;

	gs::GraphBuilder builder;
	gs::IFunctionNode& entry = builder.AddFunction("NameOfEntry");
	gs::IDataSocketDefT<float>* param1 = entry.AddArgument<float>("NameOfParameter");
	gs::IVariableDefT<float>* var = builder.AddVariable<float>("NameOfVariable");
	// 
	auto multiplyNodeBuilder = gs::CreateUnique<gs::ICustomNode>();
	gs::IDataSocketDefT<float>* inputParam = multiplyNodeBuilder->AddInput<float>("input");
	gs::IDataSocketDefT<float>* multipleParam = multiplyNodeBuilder->AddInput<float>("multiple");
	gs::IDataSocketDefT<float>* resultDef = multiplyNodeBuilder->AddOutput<float>("result");
	multiplyNodeBuilder->AddFunctionality([ & inputParam, &multipleParam, &resultDef]()
	{
			if (!inputParam->Get().has_value() || !multipleParam->Get().has_value())
			{
				return;
			}
			resultDef->Set(inputParam->Get().value() * multipleParam->Get().value());
	 
	});

	auto printFloatNodeBuilder = gs::CreateUnique<gs::ICustomNode>();
	gs::IDataSocketDefT<float>* floatInputParam = multiplyNodeBuilder->AddInput<float>("input");
	printFloatNodeBuilder->AddFunctionality([&floatInputParam]()
		{
			printf("GS : float : %f\n", floatInputParam->Get().value());
		});

	builder.AddNode(multiplyNodeBuilder.get());
	builder.AddNode(printFloatNodeBuilder.get());

	gs::IDataConnectionDefT<float>* entryToInputDef = builder.ConnectSocket<float>(param1, inputParam);
	gs::IDataConnectionDefT<float>* varToMultipleDef = builder.ConnectSocket<float>(&var->m_Socket, multipleParam);
	gs::IDataConnectionDefT<float>* outputToPrintDef = builder.ConnectSocket<float>(resultDef, floatInputParam);
	
	gs::IExecutionConnectionDef entryToMultiplyExecution = builder.ConnectNode(&entry, multiplyNodeBuilder.get());
	gs::IExecutionConnectionDef multiplyToPrintExecution = builder.ConnectNode(multiplyNodeBuilder.get(), printFloatNodeBuilder.get());
	// 
	// GSGraph instance = builder.Build();
	// GSObject object = context.AddGraph(instance);
	builder.SetVariable<float>("NameOfVariable", 3.0f);
	gs::VariableSet args;
	args["NameOfParameter"] = 3.0f;
	
	gs::HashString entryName("NameOfEntry");

	const int ITERATIONS = 100000;

	gs::Timer gsTimer;
	gsTimer.start();
	for (int i = 0; i < ITERATIONS; i++)
	{
		builder.CallFunction(entryName, args);
	}
	gsTimer.stop();

	gs::Timer cTimer;
	cTimer.start();
	for (int i = 0; i < ITERATIONS; i++)
	{
		float x = 3.0 * 3.0;
		printf("C : float : %f\n", x);
	}
	cTimer.stop();
	std::cout << "--\n-- RESULTS --\n--\n";
	std::cout << "GS Time : " << gsTimer.elapsedMilliseconds() << "ms \n";
	std::cout << "C Time : " << cTimer.elapsedMilliseconds() << "ms \n";

	// expected : 9.0f
    // Getting : 3.0f
	



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
