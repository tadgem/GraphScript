#include "GraphScript.h"
#include "BuiltInNodes.h"
using namespace gs;
const int ITERATIONS = 2;

String OpenStringAtPath(const String& path)
{
	std::ifstream t(path);
	SStream buffer;
	buffer << t.rdbuf();
	return buffer.str();

}

int main() {
	Context context;
	context.RegisterType<float>();
	context.RegisterType<bool>();
	context.RegisterType<u32>();

	auto forNodeBuilder = new ForNode();
	auto multiplyNodeBuilder = new MutliplyNodeT<float>();
	auto ifNodeBuilder = new IfNode();
	auto printFloatNodeBuilder = new PrintNodeT<float>();

	context.AddNode(forNodeBuilder);
	context.AddNode(multiplyNodeBuilder);
	context.AddNode(ifNodeBuilder);
	context.AddNode(printFloatNodeBuilder);

	String source = OpenStringAtPath("output.gs");

	GraphBuilder* builder = context.DeserializeGraph(source);
	Graph* graph = context.BuildGraph(builder);
	Graph* g2 = context.BuildGraph(builder);

	HashString entryName("NameOfEntry");

	gs::VariableSet args;
	args["NameOfParameter"] = 3.0f;
	graph->SetVariable<float>("NameOfVariable", 3.0f);
	graph->SetVariable<bool>("ConditionVariable", true);
	graph->SetVariable<u32>("LoopCount", 2);

	graph->CallFunction(entryName, args);

}
