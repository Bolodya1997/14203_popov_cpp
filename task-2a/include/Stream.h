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
    using value_type = BadType;
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
            typename _SAccessor::value_type, _T>;
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
        static_assert(std::is_same_v<int, T>);
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

    template <class Op>
    auto operator|(const Op & op) {
        return stream(op);
    }

    void print() {
        int i = 0;
        for (auto ac = begin; ac != end; ++ac) {
            if (i++ >= 10)
                break;
            std::cout << *ac << " ";
        }
        std::cout << std::endl;
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

    template <class Modifier>
    auto stream(const Modifier & modifier,
                std::enable_if_t<isModifier<Modifier>::value, void *> = nullptr) {
        auto newBegin = Accessor(begin, modifier);
        auto newEnd = Accessor(end, modifier);

        return Stream<typename Modifier::Out,
                      Container,
                      int(int),
                      Accessor<SAccessor, Modifier>>(std::move(container),
                                                     newBegin,
                                                     newEnd);
    }

    template <class Terminator>
    auto stream(const Terminator & terminator,
                std::enable_if_t<!isModifier<Terminator>::value, void *> = nullptr) {
        return terminator.terminate(begin, end);
    }

    template <class, class, class, class>
    friend class Stream;
};

#endif //STREAM_STREAM_H
