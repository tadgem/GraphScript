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