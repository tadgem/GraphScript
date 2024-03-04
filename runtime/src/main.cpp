#include "GraphScript.h"
#include "BuiltInNodes.h"
#include "GraphScript_Raylib.h"

using namespace gs;
const int ITERATIONS = 2;

String OpenStringAtPath(const String& p_UserPath)
{
	std::ifstream t(p_UserPath);
	SStream buffer;
	buffer << t.rdbuf();
	return buffer.str();

}

int main(int argc, char* argv[]) {
	String exe_name = argv[0]; // Name of the current exec program
	Vector<String> all_args;

	if (argc <= 1) {
		return -1;
	}
	Context context;
	AddRaylib(context);
	
	all_args.assign(argv + 1, argv + argc);

	// get runtime file
	// parse
	// get first variable set
	// get first instance
	// instance->CallFunction(parser.GetEntryName(), args);
}