#pragma once

#include "function_type_algorithm.h"
#include "type_list.h"
#include "function_traits.h"

namespace imp
{

namespace detail
{

template <typename...>
struct ComposeHelper;

template <typename FirstFunc>
struct ComposeHelper<FirstFunc>
{
    using type = FirstFunc;
    static FirstFunc make(FirstFunc func) { return func; }
};

template <typename FirstFunc, typename... Rest>
struct ComposeHelper<FirstFunc, Rest...>
{
    using next_func = typename ComposeHelper<Rest...>::type;
    using next_args = typename RepackFunctionArgsInto<List, next_func>::type;
    using next_return_type = typename ComposeHelper<Rest...>::return_type;
    using first_args = typename RepackFunctionArgsInto<List, FirstFunc>::type;
    using return_type = typename function_traits<FirstFunc>::return_type;

    template <typename T>
    struct BoundIsSame {
        static constexpr const bool value = std::is_convertible<next_return_type, T>::value;
    };
    template <typename T>
    struct BoundIsDifferent {
        static constexpr const bool value = not std::is_convertible<next_return_type, T>::value;
    };
    using left_first_args = typename TakeWhile<List, BoundIsDifferent, first_args>::type;
    using right_first_args = typename Tail<
        List,
        typename DropWhile<List, BoundIsDifferent, first_args>::type
        >::type;

    template <typename... LeftArgs>
    struct Op
    {
        template <typename... NextArgs>
        struct OpIncept
        {
            template <typename... RightArgs>
            struct OpInception
            {
                FirstFunc firstFunc;
                next_func nextFunc;
                OpInception(FirstFunc first, next_func next) : firstFunc(first), nextFunc(next) {}

                return_type operator()(LeftArgs... left, NextArgs... next, RightArgs... right) const
                {
                    return firstFunc(left..., nextFunc(next...), right...);
                }
            };
            using type = typename RepackInto<OpInception, right_first_args>::type;
        };
        using type = typename RepackInto<OpIncept, next_args>::type;
    };
    using op = typename RepackInto<Op, left_first_args>::type;

    static op make(FirstFunc first, Rest... rest)
    {
        return op(first, next_func(rest...));
    }
};

template <typename...>
struct ProperComposeHelper;

template <typename FirstFunc>
struct ProperComposeHelper<FirstFunc>
{
    using type = FirstFunc;
    using all_args = typename RepackFunctionArgsInto<List, FirstFunc>::type;
    static FirstFunc make(FirstFunc func) { return func; }
};
template <typename FirstFunc, typename SecondFunc, typename... Rest>
struct ProperComposeHelper<FirstFunc, SecondFunc, Rest...>
{
    using next = ProperComposeHelper<SecondFunc, Rest...>;
    using next_func = typename next::type;
    using next_args = typename next::all_args;
    using first_remaining_args = typename Tail<
        List, typename RepackFunctionArgsInto<List, FirstFunc>::type>::type;
    using all_args = typename MergeInto<List, next_args, first_remaining_args>::type;
    using return_type = typename function_traits<FirstFunc>::return_type;

    template <typename... NextArgs>
    struct OpIncept
    {
        template <typename... RightArgs>
        struct OpInception
        {
            FirstFunc firstFunc;
            next_func nextFunc;
            OpInception(FirstFunc first, next_func next) : firstFunc(first), nextFunc(next) {}

            return_type operator()(NextArgs... next, RightArgs... right) const
            {
                return firstFunc(nextFunc(next...), right...);
            }
        };
        using type = typename RepackInto<OpInception, first_remaining_args>::type;
    };
    using type = typename RepackInto<OpIncept, next_args>::type::type;

    static type make(FirstFunc first, SecondFunc second, Rest... rest)
    {
        return type(first, next::make(second, rest...));
    }
};

} //namespace detail

template <typename... Funcs>
auto compose(Funcs... funcs)
    -> decltype(detail::ProperComposeHelper<Funcs...>::make(funcs...))
{
    return detail::ProperComposeHelper<Funcs...>::make(funcs...);
}

template <typename... Funcs>
auto type_compose(Funcs... funcs)
    -> decltype(detail::ComposeHelper<Funcs...>::make(funcs...))
{
    return detail::ComposeHelper<Funcs...>::make(funcs...);
}

} //namespace imp

