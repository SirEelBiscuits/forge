#include <cstdio>
#include <cstdlib>
#include <functional>
#include <vector>
#include <sstream>

#include "forge/reflection/reflection.h"
#include "forge/reflection/jsonSerialiser.h"

bool unitTest(std::string expected, std::string got, std::string failMessage = "fail") {
	auto valid = expected.compare(got) == 0;
	if(!valid)
		std::cout << failMessage << "Expected: \"" << expected << "\"" << std::endl << "Got: \"" << got << "\"" << std::endl;
	return valid;
}

int main() {

	auto returnGood = true;

	class test{
	public:
#define ALL(NAME) NAME(x) NAME(y) NAME(z) NAME(c)
		ANV_REFLECTION_DATA()
#undef ALL

	private:
		int x {1}, y{2}, z{3};
		char c {' '};
	};

	//print names test
	{
		std::cout << "Testing printObjectMembers" << std::endl;
		std::stringstream ss;
		Reflection::printObjectMembers(test{}, ss);
		returnGood &= unitTest("x, y, z, c\n", ss.str());
	}

	//print JSON test
	{
		std::cout << "Testing printObjectAsJSON" << std::endl;
		std::stringstream ss;
		Reflection::printObjectAsJSON(test{}, ss);
		returnGood &= unitTest(
R"({
"x": 1,
"y": 2,
"z": 3,
"c": " "
})"
			, ss.str()
			, "Failed basic case. "
		);
	}

	class test2 {
	public:
#define ALL(NAME) NAME(pos) NAME(speed)
		ANV_REFLECTION_DATA()
#undef ALL
	private:
		test pos, speed;
	};

	{
		std::stringstream ss;
		Reflection::printObjectAsJSON(test2{}, ss);
		returnGood &= unitTest(
R"({
"pos": {
"x": 1,
"y": 2,
"z": 3,
"c": " "
},
"speed": {
"x": 1,
"y": 2,
"z": 3,
"c": " "
}
})"
			, ss.str()
			, "Failed Recursive case. "
		);
	}

	std::cout << (returnGood? "Success!" : "Fail") << std::endl;
	if(!returnGood)
		__debugbreak();
	return returnGood? EXIT_SUCCESS : EXIT_FAILURE;
}