#pragma once

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

} //namespace variadic_algorithm
} //namespace imp


