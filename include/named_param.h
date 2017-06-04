#pragma once

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
