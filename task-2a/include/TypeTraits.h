#ifndef STREAM_TYPETRAITS_H
#define STREAM_TYPETRAITS_H

#include <type_traits>

template <class... Args>
struct helper {
};

//  isContainer

template <class T, class V = void>
struct _isContainer : std::false_type {
};

template <class T>
struct _isContainer<T,
                    std::conditional_t<false,
                                       helper<typename std::remove_reference_t<T>::value_type,
                                              typename std::remove_reference_t<T>::const_iterator,
                                              decltype(std::declval<T>().begin()),
                                              decltype(std::declval<T>().end())>,
                                       void>
                    > : std::true_type {
};

template <class T>
inline constexpr bool isContainer = _isContainer<T>::value;

//  isIterator

template <class T, class V = void>
struct _isIterator : std::false_type {
};

template <class T>
struct _isIterator<T,
                   std::conditional_t<false,
                                      helper<decltype(*std::declval<T>()),
                                             decltype(++std::declval<T>())>,
                                      void>
                   > : std::true_type {
};

template <class T>
struct _isIterator<T *, void> : std::true_type {
};

template <class T>
inline constexpr bool isIterator = _isIterator<T>::value;

//  isGenerator

template <class T, class V = void>
struct _isGenerator : std::false_type {
};

template <class T>
struct _isGenerator<T,
                    std::conditional_t<false,
                                       decltype(std::declval<T>()()),
                                       void>
                    > : std::true_type {
};

template <class T>
inline constexpr bool isGenerator = _isGenerator<T>::value;

//  isType

template <class T>
inline constexpr bool isType = !isIterator<T> && !isContainer<T> && !isGenerator<T>;

//  isModifier

template <class T, class Accessor, class V = void>
struct _isModifier : std::false_type {
};

template <class T, class Accessor>
struct _isModifier<T, Accessor,
        std::conditional_t<false,
                decltype(std::declval<T>().modify(std::declval<Accessor>(),
                                                  std::declval<Accessor>())),
                void>
> : std::true_type {
};

template <class T, class Accessor>
inline constexpr bool isModifier = _isModifier<T, Accessor>::value;

//  isTerminator

template <class T, class Accessor, class StreamTag, class V = void>
struct _isTerminator : std::false_type {
};

template <class T, class Accessor, class StreamTag>
struct _isTerminator<T, Accessor, StreamTag,
                     std::conditional_t<false,
                                        decltype(std::declval<T>()
                                                .terminate(std::declval<Accessor>(),
                                                           std::declval<Accessor>(),
                                                           std::declval<StreamTag>())),
                                        void>
                     > : std::true_type {
};

template <class T, class Accessor, class StreamTag>
inline constexpr bool isTerminator = _isTerminator<T, Accessor, StreamTag>::value;

//  isConstRef

template <class T>
constexpr bool isConstRef = std::is_const_v<std::remove_reference_t<T>>;

//  removeCVRef

template <class T>
using removeCVRef = std::remove_cv_t<std::remove_reference_t<T>>;

#endif //STREAM_TYPETRAITS_H
