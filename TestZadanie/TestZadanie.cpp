#include <iostream>
#include <functional>
#include <type_traits>

#include "func_decorators.h"

#include <type_traits>
#include <utility>
#include <iostream>

class SimpleFuncator
{
public:
	void operator()()
	{
		std::cout << "Hello world" << std::endl;
	}
};

int main()
{
	auto lambdafoo = []() {
		std::cout << "Hello world" << std::endl;
	};

	auto lambdafoo_1 = [](auto name) {
		std::cout << "[" << name << "]" << std::endl;
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

	auto lambdafoo_4 = [](std::size_t n)
	{
		size_t sum = 0;
		for (std::size_t i = 0; i != n; ++i) {
			sum += i;
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
	//dec15();

	std::cout << std::endl;

	std::thread dec_thread;
	auto dec16 = decorate_asynchron(lambdafoo_3, false);
	dec16(3, "name");
	dec16.join();

	std::cout << std::endl;

	auto dec17 = decorate_asynchron(decorate_tracelog(decorate_printres(decorate_exception(lambdafoo_3))));
	dec17(3, "name");

	std::cout << std::endl;

	auto dec18 = decorate_asynchron(decorate_calctime(decorate_tracelog(decorate_printres(decorate_exception(lambdafoo_4)))));
	dec18(1000000);

	std::cout << std::endl;

	auto dec19 = decorate_printtime(lambdafoo_4);
	dec19(100000);

	std::cout << std::endl;

	auto dec20 = decorate_logger(decorate_calctime(decorate_tracelog(decorate_printres(decorate_printtime(lambdafoo_4)))));
	dec20(10000000);

	std::cout << std::endl;

	auto dec21 = decorate_asynchron(decorate_logger(decorate_calctime(decorate_tracelog(decorate_printres(decorate_printtime(lambdafoo_3))))));
	dec21(1000, "name");

	std::cout << std::endl;

	auto dec22 = create_customdecorate(lambdafoo, lambdafoo, lambdafoo);
	dec22();

	std::cout << std::endl;


}
