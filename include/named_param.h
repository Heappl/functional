#pragma once

template <typename T>
struct has_bracket_op
{
    template <typename U>
    static auto test(U* u) -> decltype((*u)[0])*;

    static char test(...);

    static constexpr const bool value = sizeof(test((T*)nullptr)) > 1;
};

#define NAMED_PARAM(name, param_type) \
    struct name : param_type { \
        explicit name(param_type param) : param_type(param) {} \
    }

#define SIMPLE_TYPE_NAMED_PARAM(name, param_type) \
    struct name { \
        param_type param; \
        explicit name(param_type param) : param(param) {} \
        operator const param_type&() const  { return param; } \
    }
