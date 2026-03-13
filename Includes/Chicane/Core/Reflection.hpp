#pragma once

// Macros
#ifdef __clang__
    #define CH_ENUM(...) __attribute__((annotate("CH_ENUM:" #__VA_ARGS__)))
    #define CH_TYPE(...) __attribute__((annotate("CH_TYPE:" #__VA_ARGS__)))
    #define CH_CONSTRUCTOR(...) __attribute__((annotate("CH_CONSTRUCTOR:" #__VA_ARGS__)))
    #define CH_FUNCTION(...) __attribute__((annotate("CH_FUNCTION:" #__VA_ARGS__)))
    #define CH_FIELD(...) __attribute__((annotate("CH_FIELD:" #__VA_ARGS__)))
#else
    #define CH_ENUM(...)
    #define CH_TYPE(...)
    #define CH_CONSTRUCTOR(...)
    #define CH_FUNCTION(...)
    #define CH_FIELD(...)
#endif