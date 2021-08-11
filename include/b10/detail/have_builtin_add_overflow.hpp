#ifndef B10_INCLUDED_DETAIL_HAVE_BUILTIN_ADD_OVERFLOW_HPP
#define B10_INCLUDED_DETAIL_HAVE_BUILTIN_ADD_OVERFLOW_HPP

#if defined(__has_builtin) && __has_builtin(__builtin_add_overflow)
    #define B10_DETAIL_HAVE_BUILTIN_ADD_OVERFLOW 1
#endif

#endif
