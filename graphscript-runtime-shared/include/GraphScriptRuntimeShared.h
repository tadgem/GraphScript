#ifndef GRAPHSCRIPT_RUNTIME_SHARED_H
#define GRAPHSCRIPT_RUNTIME_SHARED_H

#include "GraphScript.h"

namespace gs
{
	namespace Runtime
	{
		class Parser
		{
		public:
			Parser(Context& context, String& input);

			HashString m_EntryFunctionName;
			VariableSet m_EntrySet;
			Graph* m_Instance = nullptr;

		protected:
			enum State
			{
				Invalid = 0,
				// BeginGraphFiles/EndGraphFiles
				GraphFiles,
				// BeginEntryGraph/EndEntryGraph
				EntryGraph,
				// BeginEntryArgs/EndEntryArgs
				EntryArgs,
				// BeginFunctionName/EndFunctionName
				FunctionName,
			};

			void Parse(Context& context, String& input);

			void ParseGraphFiles(Context& context, String& input);
			void ParseEntryGraph(Context& context, String& input);
			void ParseEntryArgs(Context& context, String& input);
			void ParseFunctionName(Context& context, String& input);

			void HandleCurrentState(State& state, String& line);

		};
	}
}

#endif