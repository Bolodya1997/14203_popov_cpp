#ifndef STREAM_STREAM_H
#define STREAM_STREAM_H

#include <vector>
#include <tuple>
#include <memory>

#include "Utils.h"
#include "TypeTraits.h"
#include "GenAccessor.h"

template <class _T = BadType,
          class _Container = BadType,
          class Generator = BadType,
          class _Accessor = std::conditional_t<std::is_same_v<BadType, Generator>,
                                               typename _Container::const_iterator,
                                               GenAccessor<Generator>>,
          class StreamTag = std::conditional_t<std::is_same_v<BadType, Generator>,
                                               FiniteStreamTag,
                                               InfiniteStreamTag>,
          class... Modifiers>
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

    explicit Stream(const Generator & _generator,
                    std::enable_if_t<isGenerator<Generator>, void *> = nullptr)
            : begin(GenAccessor<Generator>(_generator)),
              end(GenAccessor<Generator>(_generator)) {
    }

    explicit Stream(Generator && _generator,
                    std::enable_if_t<isGenerator<Generator>, void *> = nullptr)
            : generator(std::move(_generator)),
              begin(GenAccessor<Generator>(generator)),
              end(GenAccessor<Generator>(generator)) {
    }

    template <class... Vs>
    explicit Stream(const _T & value, Vs &&... values)
            : Stream{ value, std::forward<Vs>(values)... } {
    }

    template <class... Vs>
    explicit Stream(_T && value, Vs &&... values)
            : Stream{ std::move(value), std::forward<Vs>(values)... } {
    }

    Stream(const Stream &) = delete;

    Stream(Stream && other) noexcept
            : container(std::move(other.container)),
              begin(std::move(other.begin)),
              end(std::move(other.end)) {
    }

    ~Stream() = default;

    template <class Modifier>
    auto operator>>(Modifier && _modifier)
    -> std::enable_if_t<isModifier<Modifier, Accessor>,
                        Stream<BadType,
                               Container,
                               Generator,
                               decltype(std::declval<Modifier>().modify(std::declval<Accessor>(),
                                                                        std::declval<Accessor>()).first),
                               std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                                 typename Modifier::StreamTag>,
                                                  FiniteStreamTag,
                                                  StreamTag>,
                               std::tuple<Modifiers...>, std::unique_ptr<Modifier>>> {
        auto modifier = std::make_unique<Modifier>(std::forward<Modifier>(_modifier));
        auto newRange = modifier->modify(begin, end);

        return Stream<BadType,
                      Container,
                      Generator,
                      decltype(std::declval<Modifier>().modify(std::declval<Accessor>(),
                                                               std::declval<Accessor>()).first),
                      std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                        typename Modifier::StreamTag>,
                                         FiniteStreamTag,
                                         StreamTag>,
                      std::tuple<Modifiers...>, std::unique_ptr<Modifier>>(std::move(container),
                                                                           std::move(generator),
                                                                           newRange.first,
                                                                           newRange.second,
                                                                           std::move(modifiers),
                                                                           std::move(modifier));
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
    Stream(Container && _container, Generator && _generator,
           Accessor _begin, Accessor _end,
           Modifiers &&... _modifiers,
           std::enable_if_t<isContainer<Container>, void *> = nullptr,
           std::enable_if_t<isGenerator<Generator>, void *> = nullptr,
           std::enable_if_t<isAccessor<Accessor>, void *> = nullptr)
            : container(std::move(_container)),
              generator(std::move(_generator)),
              begin(_begin),
              end(_end),
              modifiers(std::move(_modifiers)...) {
    }

    Container container;
    Generator generator;

    Accessor begin;
    Accessor end;

    std::tuple<Modifiers...> modifiers;

    template <class, class, class, class, class, class...>
    friend class Stream;
};

#endif //STREAM_STREAM_H
