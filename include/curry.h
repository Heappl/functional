#pragma once

#include <type_traits>
#include <utility>
#include <iostream>
#include <typeinfo>
 
using namespace std;

template <typename... Arg> struct Variadic {};

template <template <typename... Args> class Container, typename Left, typename Right>
struct MergeInto
{
    template <template <typename... FirstArgs> class First,
              template <typename... SecondArgs> class Second,
              typename... AuxLeft,
              typename... AuxRight>
    static Container<AuxLeft..., AuxRight...> merge_helper(First<AuxLeft...>*, Second<AuxRight...>*);

    using type = decltype(merge_helper((Left*)nullptr, (Right*)nullptr));
};

template <template <typename... Args> class Applicable, typename... AppliedArgs>
struct ApplyInto
{
    using type = Applicable<AppliedArgs...>;
};

template <template <typename... Args> class Container, typename VariadicType, typename... Initial>
struct RepackInto
{
    template <template <typename... SourceArgs> class Source,
              typename... ActualSourceArgs>
    static Container<Initial..., ActualSourceArgs...> helper(Source<ActualSourceArgs...>*);
    using type = decltype(helper((VariadicType*)nullptr));
};

template <typename List>
struct Size
{
    template <template <typename...> class Container, typename... Args>
    static constexpr int size(Container<Args...>*) { return sizeof...(Args); }
    static constexpr const int value = size((List*)nullptr);
};


template <bool value, typename First, typename Second>
struct Choose;
template <typename First, typename Second>
struct Choose<false, First, Second> { using type = Second; };
template <typename First, typename Second>
struct Choose<true, First, Second> { using type = First; };

template <template <typename... Args> class Container,
          template <typename T> class Predicate,
          typename... Args>
struct TakeWhile;

template <template <typename... Args> class Container,
          template <typename T> class Predicate>
struct TakeWhile<Container, Predicate>
{
    using type = Container<>;
};

template <template <typename... Args> class Container,
          template <typename T> class Predicate,
          typename Head,
          typename... Tail>
struct TakeWhile<Container, Predicate, Head, Tail...>
{
    using type = typename Choose<
        Predicate<Head>::value,
        typename MergeInto<Container, Container<Head>, typename TakeWhile<Container, Predicate, Tail...>::type>::type,
        Container<>
        >::type;
};

template <template <typename... Args> class Container,
          template <typename T> class Predicate,
          typename... Args>
struct DropWhile;

template <template <typename... Args> class Container,
          template <typename T> class Predicate>
struct DropWhile<Container, Predicate>
{
    using type = Container<>;
};
template <template <typename... Args> class Container,
          template <typename T> class Predicate,
          typename Head,
          typename... Tail>
struct DropWhile<Container, Predicate, Head, Tail...>
{
    using type = typename Choose<
        Predicate<Head>::value,
        typename MergeInto<Container, Container<>, typename DropWhile<Container, Predicate, Tail...>::type>::type,
        Container<Head, Tail...>
        >::type;
};

template <template <typename... Args> class Container,
          template <typename T> class Predicate,
          typename... Args>
struct Filter;

template <template <typename... Args> class Container,
          template <typename T> class Predicate>
struct Filter<Container, Predicate>
{
    using type = Container<>;
};
template <template <typename... Args> class Container,
          template <typename T> class Predicate,
          typename Head,
          typename... Tail>
struct Filter<Container, Predicate, Head, Tail...>
{
    using type = 
        typename MergeInto<
            Container,
            typename Choose<Predicate<Head>::value, Container<Head>, Container<>>::type,
            typename Filter<Container, Predicate, Tail...>::type
        >::type;
};

template <template <typename... Args> class Container,
          template <typename T> class Func,
          typename... Args>
struct Transform;

template <template <typename... Args> class Container,
          template <typename T> class Func>
struct Transform<Container, Func>
{
    using type = Container<>;
};
template <template <typename... Args> class Container,
          template <typename T> class Func,
          typename Head,
          typename... Tail>
struct Transform<Container, Func, Head, Tail...>
{
    using type = 
        typename MergeInto<
            Container,
            Container<typename Func<Head>::type>,
            typename Filter<Container, Func, Tail...>::type
        >::type;
};


template <typename Head, typename... Tail> struct VHead { using type = Head; };
template <typename List> struct Head { using type = typename RepackInto<VHead, List>::type; };

template <template <typename... ContainerArgs> class Container, typename... Tail>
struct VTailInto;

template <template <typename... ContainerArgs> class Container, typename Head, typename... Tail>
struct VTailInto<Container, Head, Tail...>
{
    using type = Container<Tail...>;
};
template <template <typename... ContainerArgs> class Container>
struct VTailInto<Container>
{
    using type = Container<>;
};

template <template <typename... ContainerArgs> class Container, typename List>
struct TailInto
{
    template <template <typename...> class InputContainer, typename... Elems>
    static typename VTailInto<Container, Elems...>::type helper(InputContainer<Elems...>*);
    
    using type = decltype(helper((List*)nullptr));
};

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

    using left_args = typename TakeWhile<List, is_different, Args...>::type;
    using right_args_aux = typename DropWhile<List, is_different, Args...>::type;
    using right_args = typename TailInto<List, right_args_aux>::type;
    using applied_args = typename MergeInto<List, left_args, right_args>::type;
    using left_applied = typename RepackInto<ApplyByTypeLeft, left_args, Functor, ReturnType, TypeApplied>::type;
    static_assert(Size<applied_args>::value < sizeof...(Args), "No parameter has this type");

    using type = typename RepackInto<ApplyByTypeRight, applied_args, left_applied, ReturnType>::type;
};

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
    -> decltype(curry_helper(func, &Functor::operator()))
{
    return curry_helper(func, &Functor::operator());
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
 
namespace detail
{
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
 
