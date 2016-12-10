#pragma once

#include <iostream>
#include <type_traits>

#define ANV_NAME_STRINGS(name) #name,
#define ANV_POINTERS(name) &std::remove_reference<decltype(*this)>::type::name,
#define ANV_REFLECTION_DATA() \
	constexpr auto getNames() const { return std::make_tuple(ALL(ANV_NAME_STRINGS) "" ); } \
	constexpr auto getPointers() const { return std::make_tuple(ALL(ANV_POINTERS) nullptr); }

namespace Reflection {

	namespace Detail {
		template<int n, int max, bool done = n==max>
		struct printNames_impl {
			template<typename... T>
			static void f(std::tuple<T...> &names, std::ostream os);
		};

		template<int n, int max>
		struct printNames_impl<n, max, false> {
			template<typename... T>
			static void f(std::tuple<T...> &names, std::ostream &os) {
				os << std::get<n>(names) << ", ";
				printNames_impl<n+1u, max>::f(names, os);
			}
		};

		template<int n, int max>
		struct printNames_impl<n, max, true> {
			template<typename... T>
			static void f(std::tuple<T...> &names, std::ostream &os) {
				os << std::get<n>(names) << std::endl;
			}
		};

		template<typename... T>
		void printNames(std::tuple<T...> &&names, std::ostream & os) {
			//using -2 so we can special case the last valid value, to not have a comma
			Detail::printNames_impl<0, sizeof...(T) - 2>::f(names, os);
		}
	}

	template<typename T>
	void printObjectMembers(T const & obj, std::ostream &os = std::cout) {
		Detail::printNames(obj.getNames(), os);
	}
}