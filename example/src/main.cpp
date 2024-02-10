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
	gs::IDataSocketDef* param1 = entry.AddArgument<float>("NameOfParameter");
	gs::IVariableDef* var = builder.AddVariable<float>("NameOfVariable");
	// 
	gs::INodeBuilder multiplyNodeBuilder;
	gs::IDataSocketDef* inputParam = multiplyNodeBuilder.AddInput<float>("input");
	gs::IDataSocketDef* multipleParam = multiplyNodeBuilder.AddInput<float>("multiple");
	gs::IDataSocketDef* returnDef = multiplyNodeBuilder.AddOutput<float>("result");
	// multiplyNodeBuilder.AddFunctionality((&multiplyNodeBuilder) =>
	// {
	//     float& input = multiplyNodeBuilder.GetValue<float>("input");
	//     float& multiple = multiplyNodeBuilder.GetValue<float>("multiple");
	//     float& result = multiplyNodeBuilder.GetValue<float>("result");
	//     result = input * multiple;
	// 
	// });
	//
	// INode multiplyNode = builder.AddNodeFromBuilder(multiplyNodeBuilder);
	// INode printNode = builder.AddNode(new PrintValueNode());
	// 
	// IDataConnectionDef entryToInputDef = builder.Connect<float>(param1, multiplyNode.GetConnection<float>("input"));
	// IDataConnectionDef varToMultipleDef = builder.Connect<float>(var, multiplyNode.GetConnection<float>("multiple"));
	// IDataConnectionDef outputToPrintDef = builder.Connect<float>(multiplyNode.GetConnection<float>("result"), printNode.GetConnection<float>("input"));
	//
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
