//
// Created by Admin on 4/15/2023.
//

#ifndef TYPEDEF_GUARD_H
#define TYPEDEF_GUARD_H
#include <string>
#include <sstream>
#include <fstream>
#include <any>
#include <map>
#include <functional>
#include <optional>
#include <memory>
#include <vector>
#include <stack>
#include <type_traits>
#include <iostream>


// TODO: should be a compile definition
#define GRAPH_SCRIPT_DEBUG_HASH_STRING

namespace typehash_internal
{
	static const unsigned int FRONT_SIZE = sizeof("typehash_internal::GetTypeNameHelper<") - 1u;
	static const unsigned int BACK_SIZE = sizeof(">::GetTypeName") - 1u;

	template <typename T>
	struct GetTypeNameHelper {
		static constexpr std::string GetTypeName(void) {
			static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
			std::string typeString = std::string(__FUNCTION__ + FRONT_SIZE, size - 1u);
			return typeString;
		}
	};
}

namespace gs
{
	using i8		= int8_t;
	using i16		= int16_t;
	using i32		= int32_t;
	using i64		= int64_t;
	using u8		= uint8_t;
	using u16		= uint16_t;
	using u32		= uint32_t;
	using u64		= uint64_t;
	using String	= std::string;
	using SStream	= std::stringstream;
	using FStream	= std::ofstream;
	using Any		= std::any;

	template<typename T>
	using Optional	= std::optional<T>;
	
	template<typename T, typename Y>
	using HashMap	= std::map<T, Y>;
	
	template<typename T>
	using Unique	= std::unique_ptr<T>;

	template<typename T>
	using Vector	= std::vector<T>;

	template<typename T>
	using Stack = std::stack<T>;
	
	template<typename T>
	std::ostream& operator<<(std::ostream& os, std::optional<T> const& opt)
	{
		return opt ? os << opt.value() : os << "EMPTY";
	}

	template<typename T, typename ... Args>
	constexpr Unique<T> CreateUnique(Args &&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	struct HashString {
		HashString() { m_Value = 0; }

		HashString(const String& input);
		HashString(const char* input);
		HashString(u64 value);

		u64 m_Value;

#ifdef GRAPH_SCRIPT_DEBUG_HASH_STRING
		String m_Original;
#endif

		bool operator==(HashString const& rhs) const { return m_Value == rhs.m_Value; }

		operator u64() const { return m_Value; }

	protected:
		static u64 Hash(const String& input);
	};

	template <typename T>
	constexpr std::string GetTypeName(void) {
		return typehash_internal::GetTypeNameHelper<T>::GetTypeName();
	}

	template<typename T>
	constexpr HashString GetTypeHash() {
		return HashString(GetTypeName<T>());
	}

}

#endif