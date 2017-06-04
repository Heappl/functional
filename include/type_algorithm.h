#pragma once

#include "variadic_algorithm.h"

namespace imp
{

//take variadic types from two containers
//and merge them in the given container
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

//take variadic types from some container and repack it into another
template <template <typename... Args> class Container, typename VariadicType, typename... Initial>
struct RepackInto
{
    template <template <typename... SourceArgs> class Source,
              typename... ActualSourceArgs>
    static Container<Initial..., ActualSourceArgs...> helper(Source<ActualSourceArgs...>*);
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

//if statement, choose type from two given based on given constexpr boolean value
template <bool value, typename First, typename Second>
struct If;
template <typename First, typename Second>
struct If<false, First, Second> { using type = Second; };
template <typename First, typename Second>
struct If<true, First, Second> { using type = First; };

//Take variadic types from templated type until predicate template will give true,
//and pack it into given container templated type
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
    using type = typename If<
        Predicate<Head>::value,
        typename MergeInto<Container, Container<Head>, typename TakeWhile<Container, Predicate, Tail...>::type>::type,
        Container<>
        >::type;
};

//Take from variadic types from templated, dropping all before predicate template will give true,
//and pack it into given container templated type

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
    using type = typename If<
        Predicate<Head>::value,
        typename MergeInto<Container, Container<>, typename DropWhile<Container, Predicate, Tail...>::type>::type,
        Container<Head, Tail...>
        >::type;
};

//Filter variadic types from given templated type and repack it into new container
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
            typename If<Predicate<Head>::value, Container<Head>, Container<>>::type,
            typename Filter<Container, Predicate, Tail...>::type
        >::type;
};

//Transform all types from given templated type, by applying templated functor,
//(applying means getting `typename Func<Elem>::type`)
//and put them into new containers
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

//take first type from the types in given templated type
template <typename List> struct Head
{
    using type = typename RepackInto<variadic_algorithm::Head, List>::type;
};

//take all but first type from the types in given template container
template <template <typename... ContainerArgs> class Container, typename List>
struct TailInto
{
    template <template <typename...> class InputContainer, typename... Elems>
    static typename variadic_algorithm::TailInto<Container, Elems...>::type
        helper(InputContainer<Elems...>*);
    
    using type = decltype(helper((List*)nullptr));
};

} //namespace imp


