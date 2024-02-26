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

	context.DeserializeGraph(source);
	
}
