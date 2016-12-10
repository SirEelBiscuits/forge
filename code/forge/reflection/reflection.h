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
		template<int n, int max, bool done = n==max, typename... T>
		struct printNames_impl {
			static void f(std::tuple<T...> &names, std::ostream os);
		};

		template<int n, int max, typename... T>
		struct printNames_impl<n, max, false, T...> {
			static void f(std::tuple<T...> &names, std::ostream &os) {
				os << std::get<n>(names) << ", ";
				printNames_impl<n+1u, max, n+1u == max, T...>::f(names, os);
			}
		};

		template<int n, int max, typename... T>
		struct printNames_impl<n, max, true, T...> {
			static void f(std::tuple<T...> &names, std::ostream &os) {
				os << std::get<n>(names) << std::endl;
			}
		};

		template<typename... T>
		void printNames(std::tuple<T...> &&names, std::ostream & os) {
			//using -2 so we can special case the last valid value, to not have a comma
			Detail::printNames_impl<0, sizeof...(T) - 2, 2 == sizeof...(T), T...>::f(names, os);
		}

		///////////////////////////////////////////////////////////////////////

		void printJSONLine(char const *name, int value, bool needsComma, std::ostream &os) {
			os << '"' << name << "\": " << value << (needsComma? "," : "") << std::endl;
		}

		void printJSONLine(char const *name, char value, bool needsComma, std::ostream &os) {
			os << '"' << name << "\": \"" << value << (needsComma? "\"," : "\"") << std::endl;
		}

		template<typename T>
		void printJSONLine(char const *name, T value, bool needsComma, std::ostream &os) {
			os << '"' << name << "\": ";
			printObjectAsJSON(value, os);
			os << (needsComma? "," : "") << std::endl;
		}

		template<int n, int max, bool done = n == max>
		struct printJSON_impl {
			template<typename O, typename... T, typename... S>
			static void f(O &obj, std::tuple<T...> &names, std::tuple<S...> &pointers, std::ostream &os) {
				printJSONLine(std::get<n>(names), obj.*std::get<n>(pointers), done, os);
				printJSON_impl<n+1, max, n+1 == max>::f(obj, names, pointers, os);
			}
		};

		template<int n, int max>
		struct printJSON_impl<n, max, false> {
			template<typename O, typename... T, typename... S>
			static void f(O &obj, std::tuple<T...> &names, std::tuple<S...> &pointers, std::ostream &os) {
				printJSONLine(std::get<n>(names), obj.*std::get<n>(pointers), true, os);
				printJSON_impl<n+1, max, n+1 == max>::f(obj, names, pointers, os);
			}
		};

		template<int n, int max>
		struct printJSON_impl<n, max, true> {
			template<typename O, typename... T, typename... S>
			static void f(O &obj, std::tuple<T...> &names, std::tuple<S...> &pointers, std::ostream &os) {
				printJSONLine(std::get<n>(names), obj.*std::get<n>(pointers), false, os);
			}
		};

		template<typename O, typename... T, typename... S>
		void printJSON(O &obj, std::tuple<T...> &&names, std::tuple<S...> &&pointers, std::ostream &os) {
			os << "{" << std::endl;
			printJSON_impl<0, sizeof...(T) - 2, 2 == sizeof...(T)>::f(obj, names, pointers, os);
			os << "}";
		}
	}

	template<typename T>
	void printObjectMembers(T const & obj, std::ostream &os = std::cout) {
		Detail::printNames(obj.getNames(), os);
	}

	template<typename T>
	void printObjectAsJSON(T const & obj, std::ostream &os = std::cout) {
		Detail::printJSON(obj, obj.getNames(), obj.getPointers(), os);
	}
}