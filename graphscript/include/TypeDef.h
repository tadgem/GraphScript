//
// Created by Admin on 4/15/2023.
//

#ifndef TYPEDEF_GUARD_H
#define TYPEDEF_GUARD_H
#include <string>
#include <any>
#include <map>
#include <functional>
#include <optional>
namespace gs
{
	using String = std::string;

	using Any = std::any;

	using Procedure = std::function<void()>;

	template<typename T>
	using Optional = std::optional<T>;

	template<typename T, typename Y>
	using HashMap = std::map<T, Y>;

	struct HashString {
		HashString() { m_Value = 0; }

		HashString(const String& input);
		HashString(const char* input);
		HashString(uint64_t value);

		uint64_t m_Value;

		bool operator==(HashString const& rhs) const { return m_Value == rhs.m_Value; }

		operator uint64_t() const { return m_Value; }

	protected:
		static uint64_t Hash(const String& input);
	};
}

#endif