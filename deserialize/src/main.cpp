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

	g2->SetVariable<float>("NameOfVariable", 5.0f);
	g2->SetVariable<bool>("ConditionVariable", true);
	g2->SetVariable<u32>("LoopCount", 4);

	graph->CallFunction(entryName, args);
	g2->CallFunction(entryName, args);	

	gs::VariableSet args2;
	args2["NameOfParameter"] = 6.0f;

	graph->SetVariable<float>("NameOfVariable", 6.0f);
	graph->SetVariable<bool>("ConditionVariable", true);
	graph->SetVariable<u32>("LoopCount", 4);

	g2->SetVariable<float>("NameOfVariable", 8.0f);
	g2->SetVariable<bool>("ConditionVariable", true);
	g2->SetVariable<u32>("LoopCount", 2);

	graph->CallFunction(entryName, args2);
	g2->CallFunction(entryName, args2);

	graph->SetVariable<float>("NameOfVariable", 3.0f);
	graph->SetVariable<bool>("ConditionVariable", true);
	graph->SetVariable<u32>("LoopCount", 2);

	g2->SetVariable<float>("NameOfVariable", 5.0f);
	g2->SetVariable<bool>("ConditionVariable", true);
	g2->SetVariable<u32>("LoopCount", 4);

	graph->CallFunction(entryName, args);
	g2->CallFunction(entryName, args);
}
