#include "GraphScript.h"

gs::HashString::HashString(const String& input) : m_Value(Hash(input)) {
}

gs::HashString::HashString(const char* input) : m_Value(Hash(String(input))) {
}

gs::HashString::HashString(uint64_t value) : m_Value(value)
{
}
uint64_t gs::HashString::Hash(const String& input) {
	uint64_t r = 0;
	for (int i = 0; i < input.size(); i++) {
		char c = input[i];
		r ^= 397 * c;
	}
	return r;
}

gs::IFunctionDef& gs::GraphBuilder::AddFunction(HashString functionName)
{
	// TODO: insert return statement here
	if (m_Functions.find(functionName) == m_Functions.end())
	{
		m_Functions[functionName] = IFunctionDef();
	}

	return m_Functions[functionName];
}
