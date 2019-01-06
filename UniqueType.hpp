#pragma once

namespace rib::unique
{

#define DECLARE_UNIQUE_TYPE(T) \
    struct T                   \
    {                          \
    };

DECLARE_UNIQUE_TYPE(Type0);
DECLARE_UNIQUE_TYPE(Type1);
DECLARE_UNIQUE_TYPE(Type2);
DECLARE_UNIQUE_TYPE(Type3);
DECLARE_UNIQUE_TYPE(Type4);
DECLARE_UNIQUE_TYPE(Type5);
DECLARE_UNIQUE_TYPE(Type6);
DECLARE_UNIQUE_TYPE(Type7);

#undef DECLARE_UNIQUE_TYPE

} // namespace rib::unique
