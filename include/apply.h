#pragma once

#include "type_algorithm.h"

namespace imp
{
namespace detail
{
template <typename Functor, typename ReturnType, typename Middle, typename... LeftArgs>
struct ApplyByTypeLeft
{
    using Func = Functor;
    using Applied = Middle;
    Functor func;
    Middle bound;
    ApplyByTypeLeft(Functor func, Middle bound) : func(func), bound(bound) {}

    template <typename... Args>
    ReturnType operator()(LeftArgs... left, Args... right) const
    {
        return func(left..., bound, right...);
    }
};

template <typename LeftApplied, typename ReturnType, typename... AfterArgs>
struct ApplyByTypeRight
{
    using Functor = typename LeftApplied::Func;
    using Applied = typename LeftApplied::Applied;
    LeftApplied applied;
    explicit ApplyByTypeRight(Functor func, Applied arg) : applied(func, arg) {}

    ReturnType operator()(AfterArgs... args) const
    {
        return applied(args...);
    }
};

template <typename Functor, typename ReturnType, typename TypeApplied, typename... Args>
struct ApplyByType
{
    template <typename... Types> struct List {};

    template <typename T>
    struct is_different
    {
        static constexpr const bool value = not std::is_convertible<TypeApplied, T>::value;
    };

    using left_args = typename imp::TakeWhile<List, is_different, Args...>::type;
    using right_args_aux = typename imp::DropWhile<List, is_different, Args...>::type;
    using right_args = typename imp::TailInto<List, right_args_aux>::type;
    using applied_args = typename imp::MergeInto<List, left_args, right_args>::type;
    using left_applied = typename imp::RepackInto<ApplyByTypeLeft, left_args, Functor, ReturnType, TypeApplied>::type;
    static_assert(imp::Size<applied_args>::value < sizeof...(Args), "No parameter has this type");

    using type = typename imp::RepackInto<ApplyByTypeRight, applied_args, left_applied, ReturnType>::type;
};

template <typename Functor, typename ReturnType, typename... Args>
struct Applicable;

template <typename Functor, typename ReturnType, typename FirstArg, typename... RestArgs>
struct Applicable<Functor, ReturnType, FirstArg, RestArgs...>
{
    Functor func;
    Applicable(Functor func) : func(func) {}

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
         
    Applicable<Op, ReturnType, RestArgs...> operator<<(FirstArg param) const
    {
        return Op(func, param);
    }

    template <typename SomeArgType>
    auto operator<<(SomeArgType param) const
        -> typename imp::RepackInto<
            Applicable,
            typename ApplyByType<Functor, ReturnType, SomeArgType, FirstArg, RestArgs...>::applied_args,
            typename ApplyByType<Functor, ReturnType, SomeArgType, FirstArg, RestArgs...>::type,
            ReturnType>::type 
    {
        return typename ApplyByType<Functor, ReturnType, SomeArgType, FirstArg, RestArgs...>::type(func, param);
    }
             
    auto operator()(FirstArg first, RestArgs... rest) const
        -> decltype(func(first, rest...))
    {
        return func(first, rest...);
    }
};

template <typename Functor, typename ReturnType, typename FirstArg>
struct Applicable<Functor, ReturnType, FirstArg>
{
    Functor func;
    Applicable(Functor func) : func(func) {}
         
    ReturnType operator<<(FirstArg param) const
    {
        return func(param);
    }
         
    ReturnType operator()(FirstArg first) const
    {
        return func(first);
    }
};

template <typename Functor, typename ReturnType>
ReturnType apply_helper(Functor func, ReturnType (Functor::*)() const)
{
    return func();
}

template <typename Functor, typename ReturnType, typename... Args>
detail::Applicable<Functor, ReturnType, Args...>
    apply_helper(Functor func, ReturnType (Functor::*)(Args...) const)
{
    return func;
}

template <typename Functor, typename ReturnType, typename... Args>
detail::Applicable<Functor, ReturnType, Args...>
    apply_helper(Functor func, ReturnType (Functor::*)(Args...))
{
    return func;
}

template <typename Result>
Result apply_all(Result result)
{
    return result;
}

template <typename Functor, typename FirstArg, typename... RestArgs>
auto apply_all(Functor func, FirstArg arg, RestArgs... rest)
    -> decltype(apply_all(func << arg, rest...))
{
    return apply_all(func << arg, rest...);
}


} //namespace detail


template <typename Functor>
auto apply(Functor func)
    -> decltype(detail::apply_helper(func, &Functor::operator()))
{
    return detail::apply_helper(func, &Functor::operator());
}

template <typename ReturnType, typename... Args>
auto apply(ReturnType (*func)(Args...))
    -> detail::Applicable<decltype(func), ReturnType, Args...>
{
    return func;
}

template <typename Functor, typename FirstArg, typename... Args>
auto apply(Functor func, FirstArg first, Args... args)
    -> decltype(detail::apply_all(apply(func), first, args...))
{
    return detail::apply_all(apply(func), first, args...);
}
 

} //namespace imp

