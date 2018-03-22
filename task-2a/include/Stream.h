#ifndef STREAM_STREAM_H
#define STREAM_STREAM_H

#include <vector>
#include <tuple>
#include <memory>
#include <type_traits>

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
class Stream;

template <class Container,
          class Generator,
          class Accessor,
          class StreamTag,
          class NewModifier,
          class... OldModifiers>
class Stream<BadType,
             Container,
             Generator,
             Accessor,
             StreamTag,
             std::unique_ptr<NewModifier>, std::tuple<OldModifiers...>>;

template <class _T,
          class _Container,
          class Generator,
          class _Accessor,
          class StreamTag,
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
           std::enable_if_t<isAccessor<_Accessor>, BadType> = BadType::instance)
            : begin(_begin),
              end(_end) {
    }

    explicit Stream(const _Container & _container,
                    std::enable_if_t<isContainer<_Container>, BadType> = BadType::instance)
            : begin(_container.begin()),
              end(_container.end()) {
    }

    explicit Stream(_Container && _container,
                    std::enable_if_t<isContainer<_Container>, BadType> = BadType::instance)
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
                    std::enable_if_t<isGenerator<Generator>, BadType> = BadType::instance)
            : begin(GenAccessor<Generator>(_generator)),
              end(GenAccessor<Generator>(_generator)) {
    }

    explicit Stream(Generator && _generator,
                    std::enable_if_t<isGenerator<Generator>, BadType> = BadType::instance)
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

    Stream(Stream &&) noexcept = default;

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
                               std::unique_ptr<Modifier>, std::tuple<Modifiers...>>> {
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
                      std::unique_ptr<Modifier>, std::tuple<Modifiers...>>(std::move(container),
                                                                           std::move(generator),
                                                                           newRange.first,
                                                                           newRange.second,
                                                                           std::move(modifier),
                                                                           std::move(modifiers));
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
    Container container;
    Generator generator;

    Accessor begin;
    Accessor end;

    std::tuple<Modifiers...> modifiers;
};

template <class Container,
          class Generator,
          class Accessor,
          class StreamTag,
          class NewModifier,
          class... OldModifiers>
class Stream<BadType,
             Container,
             Generator,
             Accessor,
             StreamTag,
             std::unique_ptr<NewModifier>, std::tuple<OldModifiers...>> {
private:
    using T = typename std::iterator_traits<Accessor>::value_type;

    using Tuple = std::tuple<std::unique_ptr<NewModifier>, std::tuple<OldModifiers...>>;

public:
    Stream(Container && _container, Generator && _generator,
           Accessor _begin, Accessor _end,
           std::unique_ptr<NewModifier> && newModifier, std::tuple<OldModifiers...> && oldModifiers)
            : container(std::move(_container)),
              generator(std::move(_generator)),
              begin(_begin),
              end(_end),
              modifiers(std::move(newModifier), std::move(oldModifiers)) {
    }

    Stream(const Stream &) = delete;
    Stream(Stream &&) noexcept = delete;

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
                   std::unique_ptr<Modifier>, Tuple>> {
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
                      std::unique_ptr<Modifier>, Tuple>(std::move(container),
                                                        std::move(generator),
                                                        newRange.first,
                                                        newRange.second,
                                                        std::move(modifier),
                                                        std::move(modifiers));
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
    Container container;
    Generator generator;

    Accessor begin;
    Accessor end;

    Tuple modifiers;
};

#endif //STREAM_STREAM_H
