#pragma once

#include "type_algorithm.h"
 
namespace imp
{
namespace detail
{
template <typename Functor, typename ReturnType, typename... Args>
struct Curryable;
     
template <typename Functor, typename ReturnType, typename FirstArg, typename... RestArgs>
struct Curryable<Functor, ReturnType, FirstArg, RestArgs...>
{
    Functor func;
    Curryable(Functor func) : func(func) {}

    struct Op
    {
        Functor func;
        typename std::remove_reference<FirstArg>::type first;
        Op(Functor func, FirstArg first) : func(func), first(first) {}

	    ReturnType operator()(RestArgs... args) const
        {
            return func(first, args...);
        }
    };

    auto operator<<(FirstArg param) const
        -> Curryable<Op, ReturnType, RestArgs...>
    {
        return Op(func, param);
    }

    ReturnType operator()(FirstArg first, RestArgs... rest) const
    {
        return func(first, rest...);
    }
};

template <typename Functor, typename ReturnType>
struct Curryable<Functor, ReturnType>
{
    Functor func;
    Curryable(Functor func) : func(func) {}

    ReturnType operator()() const
    {
        return func();
    }
};

template <typename Functor, typename ReturnType, typename... Args>
detail::Curryable<Functor, ReturnType, Args...>
    curry_helper(Functor func, ReturnType (Functor::*)(Args...) const)
{
    return func;
}

template <typename Functor, typename ReturnType, typename... Args>
detail::Curryable<Functor, ReturnType, Args...>
    curry_helper(Functor func, ReturnType (Functor::*)(Args...))
{
    return func;
}
     
template <typename Functor, typename ReturnType>
detail::Curryable<Functor, ReturnType>
    curry_helper(Functor func, ReturnType (Functor::*)() const)
{
    return func;
}

template <typename Functor, typename ReturnType>
detail::Curryable<Functor, ReturnType>
    curry_helper(Functor func, ReturnType (Functor::*)())
{
    return func;
}

template <typename Result>
Result curry_all(Result result)
{
    return result;
}

template <typename Functor, typename FirstArg, typename... RestArgs>
auto curry_all(Functor func, FirstArg arg, RestArgs... rest)
    -> decltype(curry_all(func << arg, rest...))
{
    return curry_all(func << arg, rest...);
}

} //namespace detail
 
template <typename Functor>
auto curry(Functor func)
    -> decltype(detail::curry_helper(func, &Functor::operator()))
{
    return detail::curry_helper(func, &Functor::operator());
}

template <typename ReturnType, typename... Args>
auto curry(ReturnType (*func)(Args...))
    -> detail::Curryable<decltype(func), ReturnType, Args...>
{
    return func;
}

template <typename Functor, typename FirstArg, typename... Args>
auto curry(Functor func, FirstArg first, Args... args)
    -> decltype(detail::curry_all(curry(func), first, args...))
{
    return detail::curry_all(curry(func), first, args...);
}

} //namespace imp
 
