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

//  isModifier

template <class T, class In, class StreamTag, class V = void>
struct _isModifier : std::false_type {
};

template <class T, class In, class StreamTag>
struct _isModifier<T, In, StreamTag,
                   std::conditional_t<false,
                                      decltype(std::declval<T>().modify(std::declval<In>(),
                                                                        std::declval<StreamTag>())),
                                      void>
                   > : std::true_type {
};

template <class T, class In, class StreamTag>
inline constexpr bool isModifier = _isModifier<T, In, StreamTag>::value;

//  isRangeModifier

template <class T, class SAccessor, class StreamTag, class V = void>
struct _isRangeModifier : std::false_type {
};

template <class T, class SAccessor, class StreamTag>
struct _isRangeModifier<T, SAccessor, StreamTag,
                        std::conditional_t<false,
                                           decltype(std::declval<T>()
                                                   .rangeModify(std::declval<SAccessor>(),
                                                                std::declval<SAccessor>(),
                                                                std::declval<StreamTag>())),
                                           void>
> : std::true_type {
};

template <class T, class SAccessor, class StreamTag>
inline constexpr bool isRangeModifier = _isRangeModifier<T, SAccessor, StreamTag>::value;

//  isTerminator

template <class T, class SAccessor, class StreamTag, class V = void>
struct _isTerminator : std::false_type {
};

template <class T, class SAccessor, class StreamTag>
struct _isTerminator<T, SAccessor, StreamTag,
                     std::conditional_t<false,
                                        decltype(std::declval<T>()
                                                .terminate(std::declval<SAccessor>(),
                                                           std::declval<SAccessor>(),
                                                           std::declval<StreamTag>)),
                                        void>
                     > : std::true_type {
};

template <class T, class SAccessor, class StreamTag>
inline constexpr bool isTerminator = _isTerminator<T, SAccessor, StreamTag>::value;

#endif //STREAM_TYPETRAITS_H
