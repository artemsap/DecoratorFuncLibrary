# DecoratorFuncLibrary

Данная библиотека предоставляет несколько стандартных декораторов для функторов, а также позволяет создать свой собственный декоратор

В библиотеке есть следующие стандартные декораторы:

Декоратор для вывода входных и выходных аргументов поступающих в функатор
```cpp

template<class T>
TraceLogDecoratorFunc<T> decorate_tracelog(T func)
{
	return TraceLogDecoratorFunc<T>(func);
}

```

Декоратор для вывода результата выполнения функции при вызове функатора
```cpp
template<class T>
PrintResultDecoratorFunc<T> decorate_printres(T func)
{
	return PrintResultDecoratorFunc<T>(func);
}
```

//Декоратор для отлова ошибок при вызове функатора
```cpp
template<class T>
CatchExceptionDecoratorFunc<T> decorate_exception(T func)
{
	return CatchExceptionDecoratorFunc<T>(func);
}
```

Декоратор для асинхронного вызова функатора
```cpp
template<class T>
AsynchronDecoratorFunc<T> decorate_asynchron(T func, bool autojoin = true)
{
	return AsynchronDecoratorFunc<T>(func, autojoin);
}
```

Декоратор для подсчета времени работы функатора
```cpp
template<class T>
CalcTimeDecoratorFunc<T> decorate_calctime(T func)
{
	return CalcTimeDecoratorFunc<T>(func);
}
```

Декоратор для вывода времени вызова функатора
```cpp
template<class T>
PrintTimeDecoratorFunc<T> decorate_printtime(T func)
{
	return PrintTimeDecoratorFunc<T>(func);
}
```

Декоратор, который перенаправляет все std::cout в файл (некая система логирования)
```cpp
template<class T>
LoggerDecoratorFunc<T> decorate_logger(T func)
{
	return LoggerDecoratorFunc<T>(func);
}
```

И функция для создания своего декоратора
```cpp
template <class T, class T1, class T2>
TemplateDecoratorFunc<T, T1, T2> create_customdecorate(T func, T1 before, T2 after)
{
	return TemplateDecoratorFunc<T, T1, T2>(func, before, after);
}
```
