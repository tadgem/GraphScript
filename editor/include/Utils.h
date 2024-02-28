#ifndef GRAPHSCRIPT_EDITOR_UTILS
#define GRAPHSCRIPT_EDITOR_UTILS

#include "GraphScript.h"

namespace gs
{
	namespace utils {
		void SaveStringAtPath(String& str, const String& path)
		{
			// save builder to string
			FStream outFile = FStream(path);
			outFile << str;
			outFile.close();
		}

		void SaveGraphAtPath(GraphBuilder* builder, const String& path)
		{
			// save builder to string
			String graphString = builder->Serialize();
			SaveStringAtPath(graphString, path);
		}

	}
}

#endif