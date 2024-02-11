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

	gs::Context context;
	gs::GraphBuilder builder;
	gs::IFunctionDef& entry = builder.AddFunction("NameOfEntry");
	gs::IDataSocketDefT<float>* param1 = entry.AddArgument<float>("NameOfParameter");
	gs::IVariableDefT<float>* var = builder.AddVariable<float>("NameOfVariable");
	// 
	auto multiplyNodeBuilder = gs::CreateUnique<gs::ICustomNode>();
	gs::IDataSocketDefT<float>* inputParam = multiplyNodeBuilder->AddInput<float>("input");
	gs::IDataSocketDefT<float>* multipleParam = multiplyNodeBuilder->AddInput<float>("multiple");
	gs::IDataSocketDefT<float>* resultDef = multiplyNodeBuilder->AddOutput<float>("result");
	multiplyNodeBuilder->AddFunctionality([ & inputParam, &multipleParam, &resultDef]()
	{
			resultDef->Set(inputParam->Get() * multipleParam->Get());
	 
	});

	auto printFloatNodeBuilder = gs::CreateUnique<gs::ICustomNode>();
	gs::IDataSocketDefT<float>* floatInputParam = multiplyNodeBuilder->AddInput<float>("input");
	multiplyNodeBuilder->AddFunctionality([&floatInputParam]()
		{
			printf("float : %f", floatInputParam->Get());
		});

	builder.AddNode(multiplyNodeBuilder.get());
	builder.AddNode(printFloatNodeBuilder.get());

	gs::IDataConnectionDefT<float>*entryToInputDef = builder.Connect<float>(param1, inputParam);
	gs::IDataConnectionDefT<float>* varToMultipleDef = builder.Connect<float>(&var->m_Socket, multipleParam);
	gs::IDataConnectionDefT<float>* outputToPrintDef = builder.Connect<float>(resultDef, floatInputParam);
	
	// IExecutionConnectionDef entryToMultiplyExecution = builder.AddExecutionConnection(entry, multiplyNode);
	// IExecutionConnectionDef multiplyToPrintExecution = builder.AddExecutionConnection(multiplyNode, printNode);
	// 
	// GSGraph instance = builder.Build();
	// GSObject object = context.AddGraph(instance);
	// object.SetVariable("NameOfVariable", 3.0f):
	// 
	// GSArgs args;
	// args["NameOfParameter"] = 3.0f;
	// object.CallFunction("NameOfEntry", args);
	// expected output = 9.0f;




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
