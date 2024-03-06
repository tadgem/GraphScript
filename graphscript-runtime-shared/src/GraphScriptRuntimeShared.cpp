#include "GraphScriptRuntimeShared.h"
#include "RuntimeUtils.h"

gs::Runtime::Parser::Parser(Context& context, String& input)
{
	Parse(context, input);
}

void gs::Runtime::Parser::Parse(Context& context, String& input)
{
	Vector<String> lines = utils::SplitStringByChar(input, '\n');
	Unique<GraphBuilder> graphBuilder = CreateUnique<GraphBuilder>(&context, "EMPTY");

	State s = State::Invalid;

	for (String& l : lines)
	{
		State previous = s;
		utils::Trim(l);
		HandleCurrentState(s, l);
		if (s == State::Invalid)
		{
			continue;
		}

		if (previous == State::Invalid)
		{
			continue;
		}

		switch (s)
		{
		case GraphFiles:
			ParseGraphFiles(context, l);
			break;
		case EntryGraph:
			ParseEntryGraph(context, l);
			break;
		case EntryArgs:
			ParseEntryArgs(context, l);
			break;
		case Invalid:
			break;
		}

	}
}

void gs::Runtime::Parser::ParseGraphFiles(Context& context, String& input)
{
}

void gs::Runtime::Parser::ParseEntryGraph(Context& context, String& input)
{
}

void gs::Runtime::Parser::ParseEntryArgs(Context& context, String& input)
{
}
