#pragma once

#include <tuple>

namespace imp
{
template <typename F>    
struct function_traits : function_traits<decltype(&F::operator())>
{
};
 
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)>
{
	using return_type = ReturnType;
    static const constexpr int arity = sizeof...(Args);
             
    template <int x>
    using argument_type = decltype(std::get<x>(*(std::tuple<Args...>*)nullptr));
};
 
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType(Args...)> : function_traits<ReturnType (*)(Args...)>
{
};
 
template <typename T, typename ReturnType, typename... Args>
struct function_traits<ReturnType (T::*)(Args...)> : function_traits<ReturnType(Args...)>
{
};
 
template <typename T, typename ReturnType, typename... Args>
struct function_traits<ReturnType (T::*)(Args...) const> : function_traits<ReturnType(Args...)>
{
};

template <typename Functor>
struct is_invocable
{
    template <typename F>
    static auto test(F* f) -> decltype((*f)())*;
         
    template <typename F>
    static auto test(F** f) -> decltype((**f)())*;
     
    static char test(...);
             
    static const bool value = sizeof(is_invocable::test((Functor*)nullptr)) > 1;
};
 
template <typename Functor>
struct has_nonambigous_call_operator
{
    template <typename F>
    static decltype(&F::operator()) test(F*);
         
    static char test(...);
             
    static const bool value = sizeof(test((Functor*)nullptr)) > 1;
};
} //namespace imp

