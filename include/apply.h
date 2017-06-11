#pragma once

#include "type_algorithm.h"
#include "type_list.h"

namespace imp
{
namespace detail
{

template <typename Functor, typename ReturnType, typename TypeApplied, typename... Args>
struct ApplyByType
{
    template <typename T>
    struct is_different
    {
        static constexpr const bool value = not std::is_convertible<TypeApplied, T>::value;
    };
  
    using all_args = List<Args...>;
    using left_args = typename TakeWhile<List, is_different, all_args>::type;
    using right_args = typename Tail<List,
        typename DropWhile<List, is_different, all_args>::type>::type;
    using remaining_args = typename MergeInto<List, left_args, right_args>::type;

    template <typename... Left>
    struct Op
    {
        template <typename... Right>
        struct OpInception
        {
            Functor func;
            TypeApplied bound;
            OpInception(Functor func, TypeApplied bound) : func(func), bound(bound) {}
            ReturnType operator()(Left... left, Right... right) const
            {
                return func(left..., bound, right...);
            }
        };
        using type = typename RepackInto<OpInception, right_args>::type;
    };
    using type = typename RepackInto<Op, left_args>::type::type;
    static_assert(Size<remaining_args>::value < sizeof...(Args),
                 "No parameter has this type");
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
        -> typename RepackInto<
            Applicable,
            typename ApplyByType<Functor, ReturnType, SomeArgType, FirstArg, RestArgs...>::remaining_args,
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

