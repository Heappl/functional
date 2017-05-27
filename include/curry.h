#include <type_traits>
#include <utility>
 
using namespace std;
 
 
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
    static decltype(&F::operator()) test(F* f);
         
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

    auto operator<<(FirstArg param) const
    {
        auto aux = func;
        auto bound = [aux, param](RestArgs... args){ return aux(param, args...); };
        return Curryable<decltype(bound), ReturnType, RestArgs...>(bound);
    }

    auto operator()(FirstArg first, RestArgs... rest) const
    {
        return func(first, rest...);
    }
};

template <typename Functor, typename ReturnType>
struct Curryable<Functor, ReturnType>
{
    Functor func;
    Curryable(Functor func) : func(func) {}

    auto operator()() const
    {
        return func();
    }
};

template <typename Functor, typename ReturnType, typename... Args>
auto curry_helper(Functor func, ReturnType (Functor::*)(Args...) const)
{
    return detail::Curryable<Functor, ReturnType, Args...>(func);
}

template <typename Functor, typename ReturnType, typename... Args>
auto curry_helper(Functor func, ReturnType (Functor::*)(Args...))
{
    return detail::Curryable<Functor, ReturnType, Args...>(func);
}
     
template <typename Functor, typename ReturnType>
auto curry_helper(Functor func, ReturnType (Functor::*)() const)
{
    return detail::Curryable<Functor, ReturnType>(func);
}

template <typename Functor, typename ReturnType>
auto curry_helper(Functor func, ReturnType (Functor::*)())
{
    return detail::Curryable<Functor, ReturnType>(func);
}

template <typename Result>
auto curry_all(Result result)
{
    return result;
}

template <typename Functor, typename FirstArg, typename... RestArgs>
auto curry_all(Functor func, FirstArg arg, RestArgs... rest)
{
    return curry_all(func << arg, rest...);
}

} //namespace detail
 
template <typename Functor>
auto curry(Functor func)
{
    return curry_helper(func, &Functor::operator());
}

template <typename ReturnType, typename... Args>
auto curry(ReturnType (*func)(Args...))
{
    return detail::Curryable<decltype(func), ReturnType, Args...>(func);
}

template <typename Functor, typename... Args>
auto curry(Functor func, Args... args)
{
    return detail::curry_all(curry(func), args...);
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
         
    auto operator<<(FirstArg param) const
    {
        auto aux = func;
        auto bound = [aux, param](RestArgs... args){ return aux(param, args...); };
        return Applicable<decltype(bound), ReturnType, RestArgs...>(bound);
    }
             
    auto operator()(FirstArg first, RestArgs... rest) const
    {
        return func(first, rest...);
    }
};

template <typename Functor, typename ReturnType, typename FirstArg>
struct Applicable<Functor, ReturnType, FirstArg>
{
    Functor func;
    Applicable(Functor func) : func(func) {}
         
    auto operator<<(FirstArg param) const
    {
        return func(param);
    }
         
    auto operator()(FirstArg first) const
    {
        return func(first);
    }
};

template <typename Functor, typename ReturnType>
auto apply_helper(Functor func, ReturnType (Functor::*)() const)
{
    return func();
}

template <typename Functor, typename ReturnType, typename... Args>
auto apply_helper(Functor func, ReturnType (Functor::*)(Args...) const)
{
    return detail::Applicable<Functor, ReturnType, Args...>(func);
}

template <typename Functor, typename ReturnType, typename... Args>
auto apply_helper(Functor func, ReturnType (Functor::*)(Args...))
{
    return detail::Applicable<Functor, ReturnType, Args...>(func);
}

template <typename Result>
auto apply_all(Result result)
{
    return result;
}

template <typename Functor, typename FirstArg, typename... RestArgs>
auto apply_all(Functor func, FirstArg arg, RestArgs... rest)
{
    return apply_all(func << arg, rest...);
}

} //namespace detail
 
template <typename Functor>
auto apply(Functor func)
{
    return detail::apply_helper(func, &Functor::operator());
}

template <typename ReturnType, typename... Args>
auto apply(ReturnType (*func)(Args...))
{
    return detail::Applicable<decltype(func), ReturnType, Args...>(func);
}

template <typename Functor, typename... Args>
auto apply(Functor func, Args... args)
{
    return detail::apply_all(apply(func), args...);
}

#define NAMED_PARAM(name, param_type) \
    struct name { \
        param_type param; \
        explicit name(param_type param) : param(param) {} \
        operator param_type() const  { return param; } \
    }
 
