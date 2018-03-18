#ifndef STREAM_STREAM_H
#define STREAM_STREAM_H

#include <vector>
#include <iostream>
#include <tuple>

#include "TypeTraits.h"
#include "Accessor.h"
#include "GenAccessor.h"

struct BadType {
    using const_iterator = BadType;

    using difference_type = BadType;
    using value_type = BadType;
    using pointer = BadType;
    using reference = BadType;
    using iterator_category = BadType;
};

struct InfiniteStreamTag {
    static constexpr const char * tag =  "INFINITE";
};
struct FiniteStreamTag {
    static constexpr const char * tag =  "FINITE";
};

template <class _T = BadType,
          class _Container = BadType,
          class Generator = BadType,
          class _SAccessor = std::conditional_t<std::is_same_v<BadType, Generator>,
                  typename _Container::const_iterator, GenAccessor<Generator>>>
class Stream {
public:
    using T = std::conditional_t<std::is_same_v<BadType, _T>,
            typename std::iterator_traits<_SAccessor>::value_type, _T>;
    using Container = std::conditional_t<std::is_same_v<BadType, _Container>,
            std::vector<T>, _Container>;
    using SAccessor = std::conditional_t<std::is_same_v<BadType, _SAccessor>,
            typename Container::const_iterator, _SAccessor>;

    using StreamTag = std::conditional_t<std::is_same_v<BadType, Generator>,
            FiniteStreamTag, InfiniteStreamTag>;

    Stream(_SAccessor _begin, _SAccessor _end,
           std::enable_if_t<isAccessor<_SAccessor>::value, void *> = nullptr)
            : begin(_begin),
              end(_end) {
    }

    explicit Stream(const _Container & _container,
                    std::enable_if_t<isContainer<_Container>::value, void *> = nullptr)
            : begin(_container.begin()),
              end(_container.end()) {
    }

    explicit Stream(_Container && _container,
                    std::enable_if_t<isContainer<_Container>::value, void *> = nullptr)
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
                    std::enable_if_t<isGenerator<Generator>::value, void *> = nullptr)
            : begin(GenAccessor<Generator>(generator)),
              end(GenAccessor<Generator>(generator)) {
    }

    template <class... Vs>
    explicit Stream(const _T & value, const Vs &... values)
            : Stream{value, values...} {
    }

    Stream(const Stream & other) = delete;

    Stream(Stream && other) noexcept
            : container(std::move(other.container)),
              begin(std::move(other.begin)),
              end(std::move(other.end)) {
    }

    ~Stream() = default;

    template <class Modifier>
    auto operator>>(const Modifier & modifier)
    -> std::enable_if_t<isModifier<Modifier, T>::value,
                        Stream<decltype(modifier.modify(std::declval<T>())),
                               Container,
                               BadType,
                               Accessor<SAccessor, Modifier>>> {
        auto newBegin = Accessor<SAccessor, Modifier>(begin, modifier);
        auto newEnd = Accessor<SAccessor, Modifier>(end, modifier);

        return Stream<decltype(modifier.modify(std::declval<T>())),
                      Container,
                      BadType,
                      Accessor<SAccessor, Modifier>>(std::move(container),
                                                     newBegin,
                                                     newEnd);
    }

    template <class Terminator>
    auto operator>>(const Terminator & terminator)
    -> std::enable_if_t<isTerminator<Terminator, SAccessor>::value,
                        decltype(terminator.terminate(std::declval<SAccessor>(),
                                                      std::declval<SAccessor>()))> {
        return terminator.terminate(begin, end);
    }

private:
    Stream(Container && container, SAccessor begin, SAccessor end,
           std::enable_if_t<isContainer<Container>::value, void *> = nullptr,
           std::enable_if_t<isAccessor<SAccessor>::value, void *> = nullptr)
            : container(std::move(container)),
              begin(begin),
              end(end) {
    }

    Container container;

    SAccessor begin;
    SAccessor end;

    template <class, class, class, class>
    friend class Stream;
};

#endif //STREAM_STREAM_H
