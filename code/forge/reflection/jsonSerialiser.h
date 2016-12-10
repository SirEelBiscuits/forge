#pragma once

#include <iostream>
#include <type_traits>

namespace Reflection {
	namespace Detail {

		/*
		 * It looks like you could use templates here, but we actually don't want to do that.
		 * using overloads, we can catch all the basic types, and templates can be safely used
		 * to pick up structs and classes
		 */

		void printJSONLine(char const *name, int value, bool needsComma, std::ostream &os) {
			os << '"' << name << "\": " << value << (needsComma? "," : "") << std::endl;
		}

		void printJSONLine(char const *name, char value, bool needsComma, std::ostream &os) {
			os << '"' << name << "\": \"" << value << (needsComma? "\"," : "\"") << std::endl;
		}

		void printJSONLine(char const *name, float value, bool needsComma, std::ostream &os) {
			os << '"' << name << "\": " << value << (needsComma? "," : "") << std::endl;
		}

		void printJSONLine(char const *name, double value, bool needsComma, std::ostream &os) {
			os << '"' << name << "\": " << value << (needsComma? "," : "") << std::endl;
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
				printJSON_impl<n+1u, max>::f(obj, names, pointers, os);
			}
		};

		template<int n, int max>
		struct printJSON_impl<n, max, false> {
			template<typename O, typename... T, typename... S>
			static void f(O &obj, std::tuple<T...> &names, std::tuple<S...> &pointers, std::ostream &os) {
				printJSONLine(std::get<n>(names), obj.*std::get<n>(pointers), true, os);
				printJSON_impl<n+1u, max>::f(obj, names, pointers, os);
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
			printJSON_impl<0, sizeof...(T) - 2>::f(obj, names, pointers, os);
			os << "}";
		}
	}

	template<typename T>
	void printObjectAsJSON(T const & obj, std::ostream &os = std::cout) {
		Detail::printJSON(obj, obj.getNames(), obj.getPointers(), os);
	}
}