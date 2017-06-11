#pragma once

#include "variadic_algorithm.h"
#include "type_if.h"

namespace imp
{

//take variadic types from two containers
//and merge them in the given container
template <template <typename...> class Container, typename Left, typename Right>
struct MergeInto
{
    template <template <typename...> class First,
              template <typename...> class Second,
              typename... AuxLeft,
              typename... AuxRight>
    static Container<AuxLeft..., AuxRight...> merge_helper(First<AuxLeft...>*, Second<AuxRight...>*);

    using type = decltype(merge_helper((Left*)nullptr, (Right*)nullptr));
};

//take variadic types from some container and repack it into another
template <template <typename...> class Container, typename VariadicType, typename... Initial>
struct RepackInto
{
    template <template <typename...> class Source, typename... Args>
    static Container<Initial..., Args...> helper(Source<Args...>*);
    using type = decltype(helper((VariadicType*)nullptr));
};

//number of types in a given templated type
template <typename List>
struct Size
{
    template <template <typename...> class Container, typename... Args>
    static constexpr int size(Container<Args...>*) { return sizeof...(Args); }
    static constexpr const int value = size((List*)nullptr);
};

//Apply algorithm on list and pack the results into container with initial types
template <template <typename...> class Container,
          template <typename> class Func,
          template <template <typename...> class,
                    template <typename> class,
                    typename...> class Algorithm,
          typename List,
          typename... Initial>
struct ApplyAlgorithm
{
    template <typename... Args>
    struct Aux
    {
        using taken = typename Algorithm<Container, Func, Args...>::type;
        using type = typename RepackInto<Container, taken, Initial...>::type;
    };
    using type = typename RepackInto<Aux, List>::type::type;
};

//Take elements from list till predicate is true
//and pack it into given container templated type
template <template <typename...> class Container,
          template <typename> class Predicate,
          typename List,
          typename... Initial>
struct TakeWhile
{
    using type = typename ApplyAlgorithm<
        Container, Predicate, variadic_algorithm::TakeWhileInto, List, Initial...>::type;
};

//Drop elements from list till predicate is true
//and pack it into given container templated type
template <template <typename...> class Container,
          template <typename> class Predicate,
          typename List,
          typename... Initial>
struct DropWhile
{
    using type = typename ApplyAlgorithm<
        Container, Predicate, variadic_algorithm::DropWhileInto, List, Initial...>::type;
};

//Filter list of types and repack it into new container
template <template <typename...> class Container,
          template <typename> class Predicate,
          typename List,
          typename... Initial>
struct Filter
{
    using type = typename ApplyAlgorithm<
        Container, Predicate, variadic_algorithm::FilterInto, List, Initial...>::type;
};

// Transform list of types and repack it into new container
template <template <typename...> class Container,
          template <typename> class Predicate,
          typename List,
          typename... Initial>
struct Transform
{
    using type = typename ApplyAlgorithm<
        Container, Predicate, variadic_algorithm::TransformInto, List, Initial...>::type;
};


//take first type from the types in given templated type
template <typename List> struct Head
{
    using type = typename RepackInto<variadic_algorithm::Head, List>::type;
};

//take all but first type from the types in given template container
template <template <typename...> class Container, typename List>
struct Tail
{
    template <template <typename...> class InputContainer, typename... Elems>
    static typename variadic_algorithm::TailInto<Container, Elems...>::type
        helper(InputContainer<Elems...>*);
    
    using type = decltype(helper((List*)nullptr));
};

} //namespace imp


