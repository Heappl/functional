#pragma once

namespace imp
{

//if statement, choose type from two given based on given constexpr boolean value
template <bool value, typename First, typename Second>
struct If;
template <typename First, typename Second>
struct If<false, First, Second> { using type = Second; };
template <typename First, typename Second>
struct If<true, First, Second> { using type = First; };

} //namespace imp

