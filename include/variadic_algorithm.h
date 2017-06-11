#pragma once

#include "type_if.h"

namespace imp
{
namespace variadic_algorithm
{

//take first type of variadic template list
template <typename First, typename... Tail>
struct Head { using type = First; };

//take all but first type of variadic template list
//and insert them to container
template <template <typename... ContainerArgs> class Container, typename... Tail>
struct TailInto;

template <template <typename... ContainerArgs> class Container, typename Head, typename... Tail>
struct TailInto<Container, Head, Tail...>
{
    using type = Container<Tail...>;
};
template <template <typename... ContainerArgs> class Container>
struct TailInto<Container>
{
    using type = Container<>;
};

//insert variadic types into given container
template <template <typename... Args> class Applicable, typename... AppliedArgs>
struct ApplyInto
{
    using type = Applicable<AppliedArgs...>;
};

//Take variadic types from templated type until predicate template will give true,
//and pack it into given container templated type
template <template <typename...> class Container,
          template <typename> class Predicate,
          typename... Args>
struct TakeWhileInto;

template <template <typename...> class Container,
          template <typename> class Predicate>
struct TakeWhileInto<Container, Predicate>
{
    using type = Container<>;
};

template <template <typename...> class Container,
          template <typename> class Predicate,
          typename Head,
          typename... Tail>
struct TakeWhileInto<Container, Predicate, Head, Tail...>
{
    template <typename... Rest>
    static Container<Head, Rest...> merge(Container<Rest...>*);

    using type = typename If<
        Predicate<Head>::value,
        decltype (merge((typename TakeWhileInto<Container, Predicate, Tail...>::type*)nullptr)),
        Container<>
    >::type;
};

//Take from variadic types from templated, dropping all before predicate template will give true,
//and pack it into given container templated type

template <template <typename... Args> class Container,
          template <typename T> class Predicate,
          typename... Args>
struct DropWhileInto;

template <template <typename... Args> class Container,
          template <typename T> class Predicate>
struct DropWhileInto<Container, Predicate>
{
    using type = Container<>;
};
template <template <typename... Args> class Container,
          template <typename T> class Predicate,
          typename Head,
          typename... Tail>
struct DropWhileInto<Container, Predicate, Head, Tail...>
{
    template <typename... Rest>
    static Container<Rest...> merge(Container<Rest...>*);

    using type = typename If<
        Predicate<Head>::value,
        decltype (merge((typename DropWhileInto<Container, Predicate, Tail...>::type*)nullptr)),
        Container<Head, Tail...>
    >::type;
};

//FilterInto variadic types from given templated type and repack it into new container
template <template <typename... Args> class Container,
          template <typename T> class Predicate,
          typename... Args>
struct FilterInto;

template <template <typename... Args> class Container,
          template <typename T> class Predicate>
struct FilterInto<Container, Predicate>
{
    using type = Container<>;
};
template <template <typename... Args> class Container,
          template <typename T> class Predicate,
          typename Head,
          typename... Tail>
struct FilterInto<Container, Predicate, Head, Tail...>
{
    template <typename... Rest>
    static Container<Head, Rest...> merge(Container<Rest...>*);

    using next = typename FilterInto<Container, Predicate, Tail...>::type;
    using type = typename If<
        Predicate<Head>::value,
        decltype (merge((next*)nullptr)),
        next>::type;
};

//TransformInto all types from given templated type, by applying templated functor,
//(applying means getting `typename Func<Elem>::type`)
//and put them into new containers
template <template <typename... Args> class Container,
          template <typename T> class Func,
          typename... Args>
struct TransformInto;

template <template <typename... Args> class Container,
          template <typename T> class Func>
struct TransformInto<Container, Func>
{
    using type = Container<>;
};
template <template <typename... Args> class Container,
          template <typename T> class Func,
          typename Head,
          typename... Tail>
struct TransformInto<Container, Func, Head, Tail...>
{
    using next = typename TransformInto<Container, Func, Tail...>::type;
    template <typename... Rest>
    static Container<typename Func<Head>::type, Rest...> merge(Container<Rest...>*);
    using type = decltype(merge((next*)nullptr));
};

} //namespace variadic_algorithm
} //namespace imp


