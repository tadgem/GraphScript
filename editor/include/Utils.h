#ifndef GRAPHSCRIPT_EDITOR_UTILS
#define GRAPHSCRIPT_EDITOR_UTILS

#include "GraphScript.h"

namespace gs
{
	namespace utils {
		void SaveStringAtPath(String& str, const String& path);

		void SaveGraphAtPath(GraphBuilder* builder, const String& path);

	}
}

#endif