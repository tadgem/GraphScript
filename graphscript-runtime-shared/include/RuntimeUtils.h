#ifndef GRAPHSCRIPT_EDITOR_UTILS
#define GRAPHSCRIPT_EDITOR_UTILS

#include "GraphScript.h"

namespace gs
{

	using RuntimeVariableSet = HashMap<HashString, Variable*>;

	namespace utils {
		void SaveStringAtPath(String& str, const String& p_UserPath);

		void SaveGraphAtPath(GraphBuilder* builder, const String& p_UserPath);

		String LoadStringAtPath(const String& p_UserPath);

		Vector<String> SplitStringByChar(const String& str, char c);

		void Trim(gs::String& s);

		GraphBuilder*		ParseGraph(Context& c, String line);
		RuntimeVariableSet	ParseVariableSet(Context& c, String line);
		Graph*				ParseGraphInstance(Context& c, String line);
	}
}

#endif