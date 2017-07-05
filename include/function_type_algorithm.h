#pragma once

#include "type_list.h"
#include "type_algorithm.h"

namespace imp
{
namespace detail
{
template <typename FunctionType>
struct RepackFunctionHelper
    : RepackFunctionHelper<decltype(&FunctionType::operator())>
{
};

template <typename ReturnType, typename... Args>
struct RepackFunctionHelper<ReturnType (*)(Args...)>
{
	using type = List<ReturnType, Args...>;
};
 
template <typename ReturnType, typename... Args>
struct RepackFunctionHelper<ReturnType(Args...)>
    : RepackFunctionHelper<ReturnType (*)(Args...)>
{
};
 
template <typename T, typename ReturnType, typename... Args>
struct RepackFunctionHelper<ReturnType (T::*)(Args...)>
    : RepackFunctionHelper<ReturnType (*)(Args...)>
{
};

template <typename T, typename ReturnType, typename... Args>
struct RepackFunctionHelper<ReturnType (T::*)(Args...) const>
    : RepackFunctionHelper<ReturnType (*)(Args...)>
{
};
} //namespace detail

//repack function return type and argument types into given container
template <template <typename...> class Container, typename FunctionType, typename... Extra>
struct RepackFunctionInto
{
    using type = typename RepackInto<
        Container, 
        typename detail::RepackFunctionHelper<FunctionType>::type,
        Extra...>::type;
};
//repack function argument types into given container
template <template <typename...> class Container, typename FunctionType, typename... Extra>
struct RepackFunctionArgsInto
{
    using type = typename RepackInto<
        Container, 
        typename Tail<
            List,
            typename detail::RepackFunctionHelper<FunctionType>::type>::type,
        Extra...>::type;
};

} //namespace imp

