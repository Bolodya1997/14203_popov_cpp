#ifndef STREAM_TYPETRAITS_H
#define STREAM_TYPETRAITS_H

#include <type_traits>

template <class... Args>
struct helper {
};

//  methodType

#define methodType(T, method, In)   \
decltype(std::declval<T>().modify(std::declval<In>()))

//  isContainer

template <class T, class V = void>
struct isContainer : std::false_type {
};

template <class T>
struct isContainer<
        T,
        std::conditional_t<
                false,
                helper<
                        typename std::remove_reference_t<T>::value_type,
                        decltype(std::declval<T>().begin()),
                        decltype(std::declval<T>().end())>,
                void
        >
> : std::true_type {
};

//  isAccessor

template <class T, class V = void>
struct isAccessor : std::false_type {
};

template <class T>
struct isAccessor<
        T,
        std::conditional_t<
                false,
                helper<
                        typename std::remove_reference_t<T>::value_type,
//                        decltype(std::declval<T>().operator==()),
//                        decltype(std::declval<T>().operator!=()),
                        decltype(std::declval<T>().operator*()),
                        decltype(std::declval<T>().operator++())>,
                void
        >
> : std::true_type {
};

//  isGenerator

template <class T, class V = void>
struct isGenerator : std::false_type {
};

template <class T>
struct isGenerator<
        T,
        std::conditional_t<
                false,
                decltype(std::declval<T>()()),
                void
        >
> : std::true_type {
};

//  isElement

template <class T, class V = void>
struct isElement {
    static constexpr bool value = !isContainer<T>::value && !isGenerator<T>::value;
};

//  isModifier

template <class T, class V = void>
struct isModifier : std::false_type {
};

template <class T>
struct isModifier<
        T,
        std::conditional_t<
                false,
                methodType(T, modify, typename T::In),
                void
        >
> : std::true_type {
};

//  isTerminator

template <class T, class V = void>
struct isTerminator : std::false_type {
};

template <class T>
struct isTerminator<
        T,
        std::conditional_t<
                false, //methodType(T, typename T::Out, terminate, typename T::In),
                helper<
                        typename T::In,
                        typename T::Out>,
                void
        >
> : std::true_type {
};

#endif //STREAM_TYPETRAITS_H
