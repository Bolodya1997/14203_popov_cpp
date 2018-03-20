#ifndef STREAM_STREAM_H
#define STREAM_STREAM_H

#include <vector>
#include <iostream>
#include <tuple>

#include "TypeTraits.h"
#include "Accessor.h"
#include "GenAccessor.h"
#include "RangeAccessor.h"

template <class _T = BadType,
          class _Container = BadType,
          class Generator = BadType,
          class _SAccessor = std::conditional_t<std::is_same_v<BadType, Generator>,
                                                typename _Container::const_iterator,
                                                GenAccessor<Generator>>,
          class StreamTag = std::conditional_t<std::is_same_v<BadType, Generator>,
                                               FiniteStreamTag,
                                               InfiniteStreamTag>>
class Stream {
    using T = std::conditional_t<std::is_same_v<BadType, _T>,
                                 typename std::iterator_traits<_SAccessor>::value_type,
                                 _T>;
    using Container = std::conditional_t<std::is_same_v<BadType, _Container>,
                                         std::vector<T>,
                                         _Container>;
    using SAccessor = std::conditional_t<std::is_same_v<BadType, _SAccessor>,
                                         typename Container::const_iterator,
                                         _SAccessor>;

public:
    Stream(_SAccessor _begin, _SAccessor _end,
           std::enable_if_t<isAccessor<_SAccessor>, void *> = nullptr)
            : begin(_begin),
              end(_end) {
    }

    explicit Stream(const _Container & _container,
                    std::enable_if_t<isContainer<_Container>, void *> = nullptr)
            : begin(_container.begin()),
              end(_container.end()) {
    }

    explicit Stream(_Container && _container,
                    std::enable_if_t<isContainer<_Container>, void *> = nullptr)
            : container(std::move(_container)),
              begin(container.begin()),
              end(container.end()) {
    }

    explicit Stream(std::initializer_list<_T> init)
            : container(std::move(Container(init))),
              begin(container.begin()),
              end(container.end()) {
    }

    explicit Stream(const Generator & generator,
                    std::enable_if_t<isGenerator<Generator>, void *> = nullptr)
            : begin(GenAccessor<Generator>(generator)),
              end(GenAccessor<Generator>(generator)) {
    }

    template <class... Vs>
    explicit Stream(const _T & value, Vs &&... values)
            : Stream{value, std::forward<Vs>(values)...} {
    }

//    template <class... Vs>
//    explicit Stream(_T && value, Vs &&... values)
//            : Stream{std::move(value), std::forward<Vs>(values)...} {
//    }

    Stream(const Stream & other) = delete;

    Stream(Stream && other) noexcept
            : container(std::move(other.container)),
              begin(std::move(other.begin)),
              end(std::move(other.end)) {
    }

    ~Stream() = default;

    template <class Modifier>
    auto operator>>(const Modifier & modifier)
    -> std::enable_if_t<isModifier<Modifier, T, StreamTag>,
                        Stream<decltype(modifier.modify(std::declval<T>(),
                                                        std::declval<StreamTag>())),
                               Container,
                               BadType,
                               Accessor<SAccessor, Modifier, StreamTag>,
                               std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                                 typename Modifier::ResultTag>,
                                                  FiniteStreamTag,
                                                  StreamTag>>> {
        auto newBegin = Accessor<SAccessor, Modifier, StreamTag>(begin, modifier);
        auto newEnd = Accessor<SAccessor, Modifier, StreamTag>(end, modifier);

        return Stream<decltype(modifier.modify(std::declval<T>(),
                                               std::declval<StreamTag>())),
                      Container,
                      BadType,
                      Accessor<SAccessor, Modifier, StreamTag>,
                      std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                        typename Modifier::ResultTag>,
                                         FiniteStreamTag,
                                         StreamTag>>(std::move(container),
                                                     newBegin,
                                                     newEnd);
    }

    template <class RangeModifier>
    auto operator>>(const RangeModifier & modifier)
    -> std::enable_if_t<isRangeModifier<RangeModifier, SAccessor, StreamTag>,
                        Stream<T,
                               Container,
                               BadType,
                               RangeAccessor<SAccessor>,
                               std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                                 typename RangeModifier::ResultTag>,
                                                  FiniteStreamTag,
                                                  StreamTag>>> {
        auto newRange = modifier.rangeModify(begin, end, StreamTag());

        return Stream<T,
                      Container,
                      BadType,
                      RangeAccessor<SAccessor>,
                      std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                        typename RangeModifier::ResultTag>,
                                         FiniteStreamTag,
                                         StreamTag>>(std::move(container),
                                                     newRange.first,
                                                     newRange.second);
    }

    template <class Terminator>
    auto operator>>(const Terminator & terminator)
    -> std::enable_if_t<isTerminator<Terminator, SAccessor, StreamTag>,
                        decltype(terminator.terminate(std::declval<SAccessor>(),
                                                      std::declval<SAccessor>(),
                                                      std::declval<StreamTag>()))> {
        return terminator.terminate(begin, end, StreamTag());
    }

private:
    Stream(Container && container, SAccessor begin, SAccessor end,
           std::enable_if_t<isContainer<Container>, void *> = nullptr,
           std::enable_if_t<isAccessor<SAccessor>, void *> = nullptr)
            : container(std::move(container)),
              begin(begin),
              end(end) {
    }

    Container container;

    SAccessor begin;
    SAccessor end;

    template <class, class, class, class, class>
    friend class Stream;
};

#endif //STREAM_STREAM_H
