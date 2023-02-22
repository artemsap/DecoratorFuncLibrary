#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <time.h>
#include <ctime>

template <class func>
class BaseDecoratorFunc
{
private:
	func functor;

	template <typename ...Args>
	using ret_type = std::result_of_t<decltype(functor)(Args...)>;
public:
	BaseDecoratorFunc(func _functor) : functor(_functor) {}

	template <typename ...Args>
	auto operator() (Args... method_args) -> ret_type<Args...>
	{
		return functor(method_args...);
	}
};

//Декоратор для вывода входных и выходных аргументов поступающих в функатор
template <class func>
class TraceLogDecoratorFunc
{
private:
	BaseDecoratorFunc<func> baseDec;

	template <typename ...Args>
	using ret_type = std::result_of_t<decltype(baseDec)(Args...)>;

	void PrintArgs()
	{
		std::cout << "No input arguments" << std::endl;
	}

	template <typename Arg>
	void PrintArgs(Arg arg)
	{
		std::cout << '[' << arg << "] " << std::endl;
	}

	template <typename Arg, typename ...Args>
	void PrintArgs(Arg arg, Args... others)
	{
		std::cout << '[' << arg << "] ";
		PrintArgs(others...);
	}

	template <typename ...Args>
	void DecoratorProcess(std::string start_message, Args... method_args)
	{
		std::cout << start_message;
		PrintArgs(method_args...);
	}

public:
	TraceLogDecoratorFunc(func _functor) : baseDec(_functor) {}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<std::is_void_v<ret_type<Args...>>>::type
	{
		DecoratorProcess("decorate_tracelog: Start calling with args: ", method_args...);
		baseDec(method_args...);
		DecoratorProcess("decorate_tracelog: End calling with args: ", method_args...);
	}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<!std::is_void_v<ret_type<Args...>>, ret_type<Args...>>::type
	{
		DecoratorProcess("decorate_tracelog: Start calling with args: ", method_args...);
		auto ret = baseDec(method_args...);
		DecoratorProcess("decorate_tracelog: End calling with args: ", method_args...);
		return ret;
	}
};

template<class T>
TraceLogDecoratorFunc<T> decorate_tracelog(T func)
{
	return TraceLogDecoratorFunc<T>(func);
}

//Декоратор для вывода результата выполнения функции при вызове функатора
template <class func>
class PrintResultDecoratorFunc
{
private:
	BaseDecoratorFunc<func> baseDec;

	template <typename ...Args>
	using ret_type = std::result_of_t<decltype(baseDec)(Args...)>;

public:
	PrintResultDecoratorFunc(func _functor) : baseDec(_functor) {}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<std::is_void_v<ret_type<Args...>>>::type
	{
		baseDec(method_args...);
		std::cout << "decorate_printres: No result. Function has void return type" << std::endl;
	}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<!std::is_void_v<ret_type<Args...>>, ret_type<Args...>>::type
	{
		auto ret = baseDec(method_args...);
		std::cout << "decorate_printres: Result of function:" << " [" << ret << ']' << std::endl;
		return ret;
	}
};

template<class T>
PrintResultDecoratorFunc<T> decorate_printres(T func)
{
	return PrintResultDecoratorFunc<T>(func);
}

//Декоратор для отлова ошибок при вызове функатора
template <class func>
class CatchExceptionDecoratorFunc
{
private:
	BaseDecoratorFunc<func> baseDec;

	template <typename ...Args>
	using ret_type = std::result_of_t<decltype(baseDec)(Args...)>;

public:
	CatchExceptionDecoratorFunc(func _functor) : baseDec(_functor) {}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<std::is_void_v<ret_type<Args...>>>::type
	{
		try
		{
			baseDec(method_args...);
		}
		catch (...)
		{
			std::clog << "ERROR IN FUNCTION" << std::endl;
		}
	}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<!std::is_void_v<ret_type<Args...>>, ret_type<Args...>>::type
	{
		try
		{
			auto ret = baseDec(method_args...);
			std::clog << "NO ERROR IN FUNCTION" << std::endl;
			return ret;
		}
		catch (...)
		{
			std::clog << "ERROR IN FUNCTION" << std::endl;
			return ret_type<Args...>();
		}
	}
};

template<class T>
CatchExceptionDecoratorFunc<T> decorate_exception(T func)
{
	return CatchExceptionDecoratorFunc<T>(func);
}

//Декоратор для асинхронного вызова функатора
template <class func>
class AsynchronDecoratorFunc
{
private:
	BaseDecoratorFunc<func> baseDec;

	template <typename ...Args>
	using ret_type = std::result_of_t<decltype(baseDec)(Args...)>;

	std::thread dec_thread;
	bool autojoin;
public:
	AsynchronDecoratorFunc(func _functor, bool _autojoin = true) : baseDec(_functor), autojoin(_autojoin) {}

	template <typename ...Args>
	static void CreateThread(typename std::decay<BaseDecoratorFunc<func>>::type baseDec, typename std::decay<Args>::type... method_args) {
		std::cout << "Thread created" << std::endl << "Thread ID : " << std::this_thread::get_id() << std::endl;
		baseDec(method_args...);
	}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<std::is_void_v<ret_type<Args...>>>::type
	{
		dec_thread = std::thread(AsynchronDecoratorFunc::CreateThread<Args...>, baseDec, method_args...);
		if (autojoin)
			dec_thread.join();
	}

	void join() {
		dec_thread.join();
	}
};

template<class T>
AsynchronDecoratorFunc<T> decorate_asynchron(T func, bool autojoin = true)
{
	return AsynchronDecoratorFunc<T>(func, autojoin);
}

//Декоратор для подсчета времени работы функатора
template <class func>
class CalcTimeDecoratorFunc
{
private:
	BaseDecoratorFunc<func> baseDec;

	template <typename ...Args>
	using ret_type = std::result_of_t<decltype(baseDec)(Args...)>;
public:
	CalcTimeDecoratorFunc(func _functor) : baseDec(_functor) {}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<std::is_void_v<ret_type<Args...>>>::type
	{
		clock_t start = clock();
		baseDec(method_args...);
		clock_t end = clock();
		double seconds = (double)(end - start) / CLOCKS_PER_SEC;
		std::cout << "decorate_calctime: time to perform the function: " << seconds << " seconds" << std::endl;
	}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<!std::is_void_v<ret_type<Args...>>, ret_type<Args...>>::type
	{
		clock_t start = clock();
		auto ret = baseDec(method_args...);
		clock_t end = clock();
		double seconds = (double)(end - start) / CLOCKS_PER_SEC;
		std::cout << "decorate_calctime: time to perform the function: " << seconds << " seconds" << std::endl;
		return ret;
	}
};

template<class T>
CalcTimeDecoratorFunc<T> decorate_calctime(T func)
{
	return CalcTimeDecoratorFunc<T>(func);
}

//Декоратор для вывода времени вызова функатора
template <class func>
class PrintTimeDecoratorFunc
{
private:
	BaseDecoratorFunc<func> baseDec;

	template <typename ...Args>
	using ret_type = std::result_of_t<decltype(baseDec)(Args...)>;
public:
	PrintTimeDecoratorFunc(func _functor) : baseDec(_functor) {}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<std::is_void_v<ret_type<Args...>>>::type
	{
		std::time_t cur_time = std::time(0);
		std::tm now;
		localtime_s(&now, &cur_time);
		std::cout << "decorate_printtime: Current time of function execution: "
			<< (now.tm_year + 1900) << '-' << (now.tm_mon + 1) << '-' << now.tm_mday << ' '
			<< now.tm_hour << ':' << now.tm_min << ':' << now.tm_sec << std::endl;
		baseDec(method_args...);
	}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<!std::is_void_v<ret_type<Args...>>, ret_type<Args...>>::type
	{
		std::time_t cur_time = std::time(0);
		std::tm now;
		localtime_s(&now, &cur_time);
		std::cout << "decorate_printtime: Current time of function execution: "
			<< (now.tm_year + 1900) << '-' << (now.tm_mon + 1) << '-' << now.tm_mday << ' '
			<< now.tm_hour << ':' << now.tm_min << ':' << now.tm_sec << std::endl;
		auto ret = baseDec(method_args...);
		return ret;
	}
};

template<class T>
PrintTimeDecoratorFunc<T> decorate_printtime(T func)
{
	return PrintTimeDecoratorFunc<T>(func);
}

//Декоратор, который перенаправляет все std::cout в файл (некая система логирования)
template <class func>
class LoggerDecoratorFunc
{
private:
	BaseDecoratorFunc<func> baseDec;

	template <typename ...Args>
	using ret_type = std::result_of_t<decltype(baseDec)(Args...)>;

	std::streambuf* oldbuf;
	std::string log_filename;

public:
	LoggerDecoratorFunc(func _functor, std::string file_name = "log.txt") : baseDec(_functor), log_filename(file_name), oldbuf(nullptr) {}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<std::is_void_v<ret_type<Args...>>>::type
	{
		std::cout << "start decorate_logger" << std::endl;
		oldbuf = std::cout.rdbuf();
		std::ofstream log_file(log_filename, std::ios::app);
		std::cout.rdbuf(log_file.rdbuf());
		baseDec(method_args...);
		std::cout.rdbuf(oldbuf);
		std::cout << "end decorate_logger" << std::endl;
	}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<!std::is_void_v<ret_type<Args...>>, ret_type<Args...>>::type
	{
		std::cout << "start decorate_logger" << std::endl;
		oldbuf = std::cout.rdbuf();
		std::ofstream log_file(log_filename);
		std::cout.rdbuf(log_file.rdbuf());
		auto ret = baseDec(method_args...);
		std::cout.rdbuf(oldbuf);
		return ret;
		std::cout << "end decorate_logger" << std::endl;
	}
};

template<class T>
LoggerDecoratorFunc<T> decorate_logger(T func)
{
	return LoggerDecoratorFunc<T>(func);
}

template <class T>
struct RetValueWrapper {
	template <class Tfunc, class... Targs>
	RetValueWrapper(Tfunc func, Targs ... args) : val(func(args...)) {}

	T value() { return val; }

private:
	T val;
};

template <>
struct RetValueWrapper<void> {
	template <class Tfunc, class... Targs>
	RetValueWrapper(Tfunc func, Targs... args) {
		func(args...);
	}

	void value() {}
};

//Модифицированная версия декоратора общего вида для создания собственных
template <class func, class beforefunc, class afterfunc>
class TemplateDecoratorFunc
{
private:
	BaseDecoratorFunc<func> baseDec;
	beforefunc before_dec;
	afterfunc after_dec;

	template <typename ...Args>
	using ret_type = std::result_of_t<decltype(baseDec)(Args...)>;
public:
	TemplateDecoratorFunc(func _functor, beforefunc _before, afterfunc _cafter) :
		baseDec(_functor), before_dec(_before), after_dec(_cafter) {}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<std::is_void_v<ret_type<Args...>>>::type
	{
		before_dec(method_args...);
		baseDec(method_args...);
		after_dec(method_args...);
	}

	template <typename ...Args>
	auto operator() (Args... method_args) -> typename std::enable_if<!std::is_void_v<ret_type<Args...>>, ret_type<Args...>>::type
	{
		before_dec(method_args...);
		auto ret = baseDec(method_args...);
		after_dec(method_args...);
		return ret;
	}

};

template <class T, class T1, class T2>
TemplateDecoratorFunc<T, T1, T2> create_customdecorate(T func, T1 before, T2 after)
{
	return TemplateDecoratorFunc<T, T1, T2>(func, before, after);
}