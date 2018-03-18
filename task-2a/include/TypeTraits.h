#ifndef STREAM_TYPETRAITS_H
#define STREAM_TYPETRAITS_H

#include <type_traits>

template <class... Args>
struct helper {
};

//  isContainer

template <class T, class V = void>
struct isContainer : std::false_type {
};

template <class T>
struct isContainer<T,
                   std::conditional_t<false,
                                      helper<typename std::remove_reference_t<T>::value_type,
                                             typename std::remove_reference_t<T>::const_iterator,
                                             decltype(std::declval<T>().begin()),
                                             decltype(std::declval<T>().end())>,
                                      void>
                   > : std::true_type {
};

//  isAccessor

template <class T, class V = void>
struct isAccessor : std::false_type {
};

template <class T>
struct isAccessor<T,
                  std::conditional_t<false,
                                     helper<typename std::iterator_traits<T>::value_type,
                                            decltype(std::declval<T>() == std::declval<T>()),
                                            decltype(std::declval<T>() != std::declval<T>()),
                                            decltype(*(std::declval<T>()))>,
//                                          decltype(++(std::declval<T>()))>,
                                     void>
                  > : std::true_type {
};

//  isGenerator

template <class T, class V = void>
struct isGenerator : std::false_type {
};

template <class T>
struct isGenerator<T,
                   std::conditional_t<false,
                                      decltype(std::declval<T>()()),
                                      void>
                   > : std::true_type {
};

//  isModifier

template <class T, class In, class V = void>
struct isModifier : std::false_type {
};

template <class T, class In>
struct isModifier<T, In,
                  std::conditional_t<false,
                                     decltype(std::declval<T>().modify(std::declval<In>())),
                                     void>
                  > : std::true_type {
};

//  isTerminator

template <class T, class SAccessor, class V = void>
struct isTerminator : std::false_type {
};

template <class T, class SAccessor>
struct isTerminator<T, SAccessor,
                    std::conditional_t<false,
                                       decltype(std::declval<T>()
                                               .terminate(std::declval<SAccessor>(),
                                                          std::declval<SAccessor>())),
                                       void>
                    > : std::true_type {
};

#endif //STREAM_TYPETRAITS_H
