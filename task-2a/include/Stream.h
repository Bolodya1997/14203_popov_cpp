#ifndef STREAM_STREAM_H
#define STREAM_STREAM_H

#include <vector>
#include <iostream>
#include <tuple>

#include "TypeTraits.h"
#include "MapAccessor.h"
#include "GenAccessor.h"
#include "RangeAccessor.h"
#include "GroupAccessor.h"

template <class _T = BadType,
          class _Container = BadType,
          class Generator = BadType,
          class _Accessor = std::conditional_t<std::is_same_v<BadType, Generator>,
                                               typename _Container::const_iterator,
                                               GenAccessor<Generator>>,
          class StreamTag = std::conditional_t<std::is_same_v<BadType, Generator>,
                                               FiniteStreamTag,
                                               InfiniteStreamTag>>
class Stream {
    using T = std::conditional_t<std::is_same_v<BadType, _T>,
                                 typename std::iterator_traits<_Accessor>::value_type,
                                 _T>;
    using Container = std::conditional_t<std::is_same_v<BadType, _Container>,
                                         std::vector<T>,
                                         _Container>;
    using Accessor = std::conditional_t<std::is_same_v<BadType, _Accessor>,
                                        typename Container::const_iterator,
                                        _Accessor>;

public:
    Stream(_Accessor _begin, _Accessor _end,
           std::enable_if_t<isAccessor<_Accessor>, void *> = nullptr)
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
            : Stream{ value, std::forward<Vs>(values)... } {
    }

//    template <class... Vs>
//    explicit Stream(_T && value, Vs &&... values)
//            : Stream{ std::move(value), std::forward<Vs>(values)... } {
//    }

    Stream(const Stream &) = delete;

    Stream(Stream && other) noexcept
            : container(std::move(other.container)),
              begin(std::move(other.begin)),
              end(std::move(other.end)) {
    }

    ~Stream() = default;

    template <class RangeModifier>
    auto operator>>(const RangeModifier & modifier)
    -> std::enable_if_t<isRangeModifier<RangeModifier, Accessor>,
                        Stream<T,
                               Container,
                               BadType,
                               RangeAccessor<Accessor>,
                               std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                                 typename RangeModifier::StreamTag>,
                                                  FiniteStreamTag,
                                                  StreamTag>>> {
        auto newRange = modifier.rangeModify(begin, end);

        return Stream<T,
                      Container,
                      BadType,
                      RangeAccessor<Accessor>,
                      std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                        typename RangeModifier::StreamTag>,
                                         FiniteStreamTag,
                                         StreamTag>>(std::move(container),
                                                     newRange.first,
                                                     newRange.second);
    }

    template <class MapModifier>
    auto operator>>(const MapModifier & modifier)
    -> std::enable_if_t<isMapModifier<MapModifier, T>,
                        Stream<decltype(modifier.mapModify(std::declval<T>())),
                               Container,
                               BadType,
                               MapAccessor<Accessor, MapModifier>,
                               std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                                 typename MapModifier::StreamTag>,
                                                  FiniteStreamTag,
                                                  StreamTag>>> {
        auto newBegin = MapAccessor<Accessor, MapModifier>(begin, modifier);
        auto newEnd = MapAccessor<Accessor, MapModifier>(end, modifier);

        return Stream<decltype(modifier.mapModify(std::declval<T>())),
                      Container,
                      BadType,
                      MapAccessor<Accessor, MapModifier>,
                      std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                        typename MapModifier::StreamTag>,
                                         FiniteStreamTag,
                                         StreamTag>>(std::move(container),
                                                     newBegin,
                                                     newEnd);
    }

    template <class GroupModifier>
    auto operator>>(const GroupModifier & modifier)
    -> std::enable_if_t<isGroupModifier<GroupModifier, Accessor>,
                        Stream<typename std::iterator_traits<GroupAccessor<Accessor>>::value_type,
                               Container,
                               BadType,
                               GroupAccessor<Accessor>,
                               std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                                 typename GroupModifier::StreamTag>,
                                                  FiniteStreamTag,
                                                  StreamTag>>> {
        auto newRange = modifier.groupModify(begin, end);

        return Stream<typename std::iterator_traits<GroupAccessor<Accessor>>::value_type,
                      Container,
                      BadType,
                      GroupAccessor<Accessor>,
                      std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                        typename GroupModifier::StreamTag>,
                                         FiniteStreamTag,
                                         StreamTag>>(std::move(container),
                                                     newRange.first,
                                                     newRange.second);
    }

    template <class Terminator>
    auto operator>>(const Terminator & terminator)
    -> std::enable_if_t<isTerminator<Terminator, Accessor, StreamTag>,
                        decltype(terminator.terminate(std::declval<Accessor>(),
                                                      std::declval<Accessor>(),
                                                      std::declval<StreamTag>()))> {
        return terminator.terminate(begin, end, StreamTag());
    }

private:
    Stream(Container && container, Accessor begin, Accessor end,
           std::enable_if_t<isContainer<Container>, void *> = nullptr,
           std::enable_if_t<isAccessor<Accessor>, void *> = nullptr)
            : container(std::move(container)),
              begin(begin),
              end(end) {
    }

    Container container;

    Accessor begin;
    Accessor end;

    template <class, class, class, class, class>
    friend class Stream;
};

#endif //STREAM_STREAM_H
