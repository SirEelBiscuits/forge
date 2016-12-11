#pragma once

#include <iostream>
#include <type_traits>

namespace Reflection {
	namespace Detail {

		/*
		 * while function overloads might be simpler here, we'd still want a
		 * way to handle the recursive case for reflectable classes, and we 
		 * need a way to separate classes, pointers, enums for special 
		 * treatment as necessary.
		 */
		template<
			typename T,
			bool isClass   = std::is_class<T>::value,
			bool isEnum    = std::is_enum<T>::value,
			bool isPointer = std::is_pointer<T>::value
		> struct printJSONLine {
			static void f(char const *name, T value, bool needsComma, std::ostream &os) {
				static_assert(false, "Type T not supported!");
			}
		};

		template<>
		struct printJSONLine<int, false, false, false> {
			static void f(char const *name, int value, bool needsComma, std::ostream &os) {
				os << '"' << name << "\": " << value << (needsComma? "," : "") << std::endl;
			}
		};
		
		template<>
		struct printJSONLine<char, false, false, false> {
			static void f(char const *name, char value, bool needsComma, std::ostream &os) {
				os << '"' << name << "\": \"" << value << (needsComma? "\"," : "\"") << std::endl;
			}
		};
		
		template<>
		struct printJSONLine<float, false, false, false> {
			static void f(char const *name, float value, bool needsComma, std::ostream &os) {
				os << '"' << name << "\": " << value << (needsComma? "," : "") << std::endl;
			}
		};
		
		template<>
		struct printJSONLine<double, false, false, false> {
			static void f(char const *name, double value, bool needsComma, std::ostream &os) {
				os << '"' << name << "\": " << value << (needsComma? "," : "") << std::endl;
			}
		};
		
		template<typename T>
		struct printJSONLine<T, true, false, false> {
			static void f(char const *name, T value, bool needsComma, std::ostream &os) {
				os << '"' << name << "\": ";
				printObjectAsJSON(value, os);
				os << (needsComma? "," : "") << std::endl;
			}
		};

		template<typename T>
		struct printJSONLine<T, false, true, false> {
			static void f(char const *name, T value, bool needsComma, std::ostream &os) {
				os << '"' << name << "\": " << (int)value << (needsComma? "," : "") << std::endl;
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<int n, int max, bool done = n == max>
		struct printJSON_impl {
			template<typename O, typename... T, typename... S>
			static void f(O &obj, std::tuple<T...> &names, std::tuple<S...> &pointers, std::ostream &os);
		};

		template<int n, int max>
		struct printJSON_impl<n, max, false> {
			template<typename O, typename... T, typename... S>
			static void f(O &obj, std::tuple<T...> &names, std::tuple<S...> &pointers, std::ostream &os) {
				printJSONLine<
					std::remove_const<std::remove_reference<decltype(obj.*std::get<n>(pointers))>::type>::type
				>::f(std::get<n>(names), obj.*std::get<n>(pointers), true, os);
				printJSON_impl<n+1u, max>::f(obj, names, pointers, os);
			}
		};

		template<int n, int max>
		struct printJSON_impl<n, max, true> {
			template<typename O, typename... T, typename... S>
			static void f(O &obj, std::tuple<T...> &names, std::tuple<S...> &pointers, std::ostream &os) {
				printJSONLine<
					std::remove_const<std::remove_reference<decltype(obj.*std::get<n>(pointers))>::type>::type
				>::f(std::get<n>(names), obj.*std::get<n>(pointers), false, os);
			}
		};

		template<
			typename O,
			typename... T,
			typename... S
		>
		void printJSON(O &obj, std::tuple<T...> &&names, std::tuple<S...> &&pointers, std::ostream &os) {
			os << "{" << std::endl;
			printJSON_impl<0, sizeof...(T) - 2>::f(obj, names, pointers, os);
			os << "}";
		}
	}

	template<typename T>
	void printObjectAsJSON(T const & obj, std::ostream &os = std::cout) {
		static_assert(std::is_class<T>::value, "invalid type passed for printObjectAsJSON - must be an object with getNames and getPointers declared!");
		Detail::printJSON(obj, obj.getNames(), obj.getPointers(), os);
	}
}