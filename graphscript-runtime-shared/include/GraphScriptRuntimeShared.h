#ifndef GRAPHSCRIPT_RAYLIB_H
#define GRAPHSCRIPT_RAYLIB_H

#include "GraphScript.h"

namespace gs
{
	namespace Runtime
	{
		class Parser
		{
		public:
			Parser(Context& context, String& input);

		protected:
			enum State
			{
				Invalid = 0,
				// BeginGraphFiles/EndGraphFiles
				GraphFiles,
				// BeginEntryGraph/EndEntryGraph
				EntryGraph,
				// BeginEntryArgs/EndEntryArgs
				EntryArgs
			};
			
			HashString GetEntryName() { return p_EntryFunctionName; }

			void Parse(Context& context, String& input);

			void ParseGraphFiles(Context& context, String& input);
			void ParseEntryGraph(Context& context, String& input);
			void ParseEntryArgs(Context& context, String& input);

			void HandleCurrentState(State& state, String& line);

			HashString p_EntryFunctionName;
		};
	}
}

#endif