#pragma once
#include <iostream>
#include <thread>

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

	template<typename Arg>
	void PrintArgs(Arg first)
	{
		std::cout << '[' << first << ']' << std::endl;
	}

	template<typename Arg, typename... Args>
	void PrintArgs(Arg first, Args... others)
	{
		std::cout << '[' << first << "] ";
		PrintArgs(others...);
	}

	template <typename ...Args>
	void DecoratorProcess(std::string start_message, Args... method_args)
	{
		std::cout << start_message;
		PrintArgs(method_args...);
	}

public:
	TraceLogDecoratorFunc(func _functor) : baseDec(_functor){}
	
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