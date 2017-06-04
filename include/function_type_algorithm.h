#pragma once

namespace imp
{

//repack function return type and argument types into given container
template <template <typename...> class Container, typename FunctionType>
struct RepackFunctionInto
    : RepackFunctionInto<Container, decltype(&FunctionType::operator())>
{
};

template <template <typename...> class Container, typename ReturnType, typename... Args>
struct RepackFunctionInto<Container, ReturnType (*)(Args...)>
{
	using all = Container<ReturnType, Args...>;
    using args = Container<Args...>;
};
 
template <template <typename...> class Container, typename ReturnType, typename... Args>
struct RepackFunctionInto<Container, ReturnType(Args...)>
    : RepackFunctionInto<Container, ReturnType (*)(Args...)>
{
};
 
template <template <typename...> class Container,
          typename T,
          typename ReturnType,
          typename... Args>
struct RepackFunctionInto<Container, ReturnType (T::*)(Args...)>
    : RepackFunctionInto<Container, ReturnType (*)(Args...)>
{
};

} //namespace imp

