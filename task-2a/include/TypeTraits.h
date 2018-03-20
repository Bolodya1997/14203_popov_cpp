#ifndef STREAM_TYPETRAITS_H
#define STREAM_TYPETRAITS_H

#include <type_traits>

struct BadType {
    using const_iterator = BadType;

    using difference_type = BadType;
    using value_type = BadType;
    using pointer = BadType;
    using reference = BadType;
    using iterator_category = BadType;
};

struct InfiniteStreamTag {
};
struct FiniteStreamTag {
};

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

//  isAccessor

template <class T, class V = void>
struct _isAccessor : std::false_type {
};

template <class T>
struct _isAccessor<T,
                   std::conditional_t<false,
                                      helper<typename std::iterator_traits<T>::value_type,
                                             decltype(std::declval<T>() == std::declval<T>()),
                                             decltype(std::declval<T>() != std::declval<T>()),
                                             decltype(*(std::declval<T>()))>,
//                                           decltype(++(std::declval<T>()))>,
                                      void>
                   > : std::true_type {
};

template <class T>
inline constexpr bool isAccessor = _isAccessor<T>::value;

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

//  isRangeModifier

template <class T, class Accessor, class V = void>
struct _isRangeModifier : std::false_type {
};

template <class T, class Accessor>
struct _isRangeModifier<T, Accessor,
        std::conditional_t<false,
                decltype(std::declval<T>().rangeModify(std::declval<Accessor>(),
                                                       std::declval<Accessor>())),
                void>
> : std::true_type {
};

template <class T, class Accessor>
inline constexpr bool isRangeModifier = _isRangeModifier<T, Accessor>::value;

//  isMapModifier

template <class T, class In, class V = void>
struct _isMapModifier : std::false_type {
};

template <class T, class In>
struct _isMapModifier<T, In,
                      std::conditional_t<false,
                                         decltype(std::declval<T>().mapModify(std::declval<In>())),
                                         void>
                      > : std::true_type {
};

template <class T, class In>
inline constexpr bool isMapModifier = _isMapModifier<T, In>::value;

//  isGroupModifier

template <class T, class Accessor, class V = void>
struct _isGroupModifier : std::false_type {
};

template <class T, class Accessor>
struct _isGroupModifier<T, Accessor,
        std::conditional_t<false,
                           decltype(std::declval<T>().groupModify(std::declval<Accessor>(),
                                                                  std::declval<Accessor>())),
                           void>
> : std::true_type {
};

template <class T, class Accessor>
inline constexpr bool isGroupModifier = _isGroupModifier<T, Accessor>::value;

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
                                                           std::declval<StreamTag>)),
                                        void>
                     > : std::true_type {
};

template <class T, class Accessor, class StreamTag>
inline constexpr bool isTerminator = _isTerminator<T, Accessor, StreamTag>::value;

#endif //STREAM_TYPETRAITS_H
