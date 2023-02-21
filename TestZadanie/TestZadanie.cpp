#include <iostream>
#include <functional>
#include <type_traits>

#include "func_decorators.h"

class SimpleFuncator
{
public:
	void operator()()
	{
		std::cout << "Hello world" << std::endl;
	}
};

void foo(std::string str)
{
	std::cout << "Hello " << str << std::endl;
}

int main()
{
	auto lambdafoo = []() {
		std::cout << "Hello world" << std::endl;
	};

	auto lambdafoo_1 = [](std::string name) {
		std::cout << "Hello " << name << std::endl;
	};

	auto lambdafoo_2 = [](std::string name) {
		return name;
	};

	auto lambdafoo_3 = [](std::size_t n, std::string const& name)
	{
		for (std::size_t i = 0; i != n; ++i) {
			std::cout << "Hello " << name << std::endl;
		};
	};

	SimpleFuncator a;

	auto dec11 = decorate_tracelog(
		[](std::size_t n, std::string const& name)
		{
			for (std::size_t i = 0; i != n; ++i) {
				std::cout << "Hello " << name << std::endl;
			};
		}
	);
	dec11(3, "name");
	
	std::cout << std::endl;

	auto dec12 = decorate_printres(lambdafoo_2);
	auto name12 = dec12("name");

	std::cout << std::endl;

	auto dec13 = decorate_tracelog(decorate_printres(lambdafoo_2));
	auto name13 = dec13("name");

	std::cout << std::endl;

	auto dec14 = decorate_tracelog(decorate_printres(a));
	dec14();

	std::cout << std::endl;

	auto dec15 = decorate_exception(
		[]()
		{
			int num;
			std::cin >> num;
			if (num < 0)
				throw - 1;
			else
				return num;
		});
	dec15();
}
