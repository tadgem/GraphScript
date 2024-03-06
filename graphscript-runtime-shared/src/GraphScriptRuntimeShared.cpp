#include "GraphScriptRuntimeShared.h"
#include "RuntimeUtils.h"

gs::Runtime::Parser::Parser(Context& context, String& input)
{
	Parse(context, input);
}

void gs::Runtime::Parser::Parse(Context& context, String& input)
{
	Vector<String> lines = utils::SplitStringByChar(input, '\n');
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
		case FunctionName:
			ParseFunctionName(context, l);
			break;
		case Invalid:
			break;
		}

	}
}

void gs::Runtime::Parser::ParseGraphFiles(Context& context, String& input)
{
	String source = utils::LoadStringAtPath(input);
	context.DeserializeGraph(source);
}

void gs::Runtime::Parser::ParseEntryGraph(Context& context, String& input)
{
	GraphBuilder* builder = context.FindBuilder(input);

	GS_ASSERT(builder != nullptr, "Could not find builder prototype");
	m_Instance = context.BuildGraph(builder);
}

void gs::Runtime::Parser::ParseEntryArgs(Context& context, String& input)
{
	RuntimeVariableSet rvs = utils::ParseVariableSet(context, input);
	m_EntrySet = utils::RuntimeToVariableSet(rvs);
}

void gs::Runtime::Parser::ParseFunctionName(Context& context, String& input)
{
	m_EntryFunctionName = input;
}


void gs::Runtime::Parser::HandleCurrentState(State& s, String& l)
{
	if (l == "BeginGraphFiles")
	{
		s = State::GraphFiles;
	}
	if (l == "EndGraphFiles")
	{
		s = State::Invalid;
	}
	if (l == "BeginFunctionName")
	{
		s = State::FunctionName;
	}
	if (l == "EndFunctionName")
	{
		s = State::Invalid;
	}
	if (l == "BeginEntryGraph")
	{
		s = State::EntryGraph;
	}
	if (l == "EndEntryGraph")
	{
		s = State::Invalid;
	}

	if (l == "BeginEntryArgs")
	{
		s = State::EntryArgs;
	}
	if (l == "EndEntryArgs")
	{
		s = State::Invalid;
	}
}
