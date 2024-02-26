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

	context.DeserializeGraph(source);
	
}
