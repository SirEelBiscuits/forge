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
		> struct printJSONValue {
			static void f(T value, std::ostream &os) {
				static_assert(false, "Type T not supported!");
			}
		};

		template<>
		struct printJSONValue<int, false, false, false> {
			static void f(int value, std::ostream &os) {
				os <<  value;
			}
		};
		
		template<>
		struct printJSONValue<char, false, false, false> {
			static void f(char value, std::ostream &os) {
				os << "\"" << value << '"';
			}
		};
		
		template<>
		struct printJSONValue<float, false, false, false> {
			static void f(float value, std::ostream &os) {
				os << value;
			}
		};
		
		template<>
		struct printJSONValue<double, false, false, false> {
			static void f(double value, std::ostream &os) {
				os << value << std::endl;
			}
		};
		
		//structs
		template<typename T>
		struct printJSONValue<T, true, false, false> {
			static void f(T value, std::ostream &os) {
				printObjectAsJSON(value, os);
			}
		};

		//enums
		template<typename T>
		struct printJSONValue<T, false, true, false> {
			static void f(T value, std::ostream &os) {
				os << (int)value;
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<typename T>
		void printJSONLine(char const *name, T value, bool needsComma, std::ostream &os = std::cout) {
			os << '"' << name << "\": ";
			printJSONValue<T>::f(value, os);
			os << (needsComma? "," : "") << std::endl;
		}

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