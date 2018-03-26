#ifndef STREAM_STREAM_H
#define STREAM_STREAM_H

#include <vector>
#include <tuple>
#include <memory>
#include <type_traits>

#include "Utils.h"
#include "TypeTraits.h"
#include "IterAccessor.h"
#include "GenAccessor.h"

template <class Container,
          class Generator,
          class Accessor,
          class StreamTag,
          class... Modifiers>
class Stream;

template <class Container,
          class Generator,
          class Accessor,
          class StreamTag,
          class NewModifier,
          class... OldModifiers>
class Stream<Container,
             Generator,
             Accessor,
             StreamTag,
             std::unique_ptr<NewModifier>, std::tuple<OldModifiers...>>;

template <class Container,
          class Generator,
          class Accessor,
          class StreamTag,
          class... Modifiers>
class Stream {
public:
    template <class Iterator>
    Stream(Iterator _begin, Iterator _end,
           std::enable_if_t<isIterator<Iterator>, BadType> = BAD_TYPE_INSTANCE)
            : begin(Accessor(_begin)),
              end(Accessor(_end)) {
    }

    explicit Stream(const Container & _container,
                    std::enable_if_t<isContainer<Container>, BadType> = BAD_TYPE_INSTANCE)
            : begin(Accessor(_container.begin())),
              end(Accessor(_container.end())) {
    }

    explicit Stream(const Container && _container,
                    std::enable_if_t<isContainer<Container>, BadType> = BAD_TYPE_INSTANCE)
            : container(std::move(_container)),
              begin(Accessor(container.begin())),
              end(Accessor(container.end())) {
    }

    template <class T>
    explicit Stream(std::initializer_list<T> init)
            : container(init),
              begin(Accessor(container.begin())),
              end(Accessor(container.end())) {
    }

    explicit Stream(const Generator & _generator,
                    std::enable_if_t<isGenerator<Generator>, BadType> = BAD_TYPE_INSTANCE)
            : begin(GenAccessor<Generator>(_generator)),
              end(GenAccessor<Generator>(_generator)) {
    }

    explicit Stream(Generator && _generator,
                    std::enable_if_t<isGenerator<Generator>, BadType> = BAD_TYPE_INSTANCE)
            : generator(std::move(_generator)),
              begin(GenAccessor<Generator>(generator)),
              end(GenAccessor<Generator>(generator)) {
    }

    template <class T, class... Vs>
    explicit Stream(T && value, Vs &&... values)
            : Stream(std::forward<Vs>(values)...) {
        container.push_back(std::forward<T>(value));
    }

    Stream(const Stream &) = delete;
    Stream(Stream &&) noexcept = default;

    ~Stream() = default;

    template <class Modifier>
    auto operator>>(Modifier && _modifier)
    -> std::enable_if_t<isModifier<Modifier, Accessor>,
                        Stream<Container,
                               Generator,
                               typename Modifier::template Accessor<Accessor>,
                               std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                                 typename Modifier::StreamTag>,
                                                  FiniteStreamTag,
                                                  StreamTag>,
                               std::unique_ptr<Modifier>,
                               std::tuple<Modifiers...>>> {
        auto modifier = std::make_unique<Modifier>(std::forward<Modifier>(_modifier));
        auto newRange = modifier->modify(begin, end);

        return Stream<Container,
                      Generator,
                      typename Modifier::template Accessor<Accessor>,
                      std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                        typename Modifier::StreamTag>,
                                         FiniteStreamTag,
                                         StreamTag>,
                      std::unique_ptr<Modifier>,
                      std::tuple<Modifiers...>>(std::move(container),
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
    Stream()
            : begin(container.begin()),
              end(container.end()) {
    }

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
class Stream<Container,
             Generator,
             Accessor,
             StreamTag,
             std::unique_ptr<NewModifier>,
             std::tuple<OldModifiers...>> {
private:
    using Tuple = std::tuple<std::unique_ptr<NewModifier>,
                             std::tuple<OldModifiers...>>;

public:
    Stream(Container && _container, Generator && _generator,
           Accessor _begin, Accessor _end,
           std::unique_ptr<NewModifier> && newModifier,
           std::tuple<OldModifiers...> && oldModifiers)
            : container(std::move(_container)),
              generator(std::move(_generator)),
              begin(_begin),
              end(_end),
              modifiers(std::move(newModifier),
                        std::move(oldModifiers)) {
    }

    Stream(const Stream &) = delete;
    Stream(Stream &&) noexcept = default;

    ~Stream() = default;

    template <class Modifier>
    auto operator>>(Modifier && _modifier)
    -> std::enable_if_t<isModifier<Modifier, Accessor>,
                        Stream<Container,
                               Generator,
                               typename Modifier::template Accessor<Accessor>,
                               std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                                 typename Modifier::StreamTag>,
                                                  FiniteStreamTag,
                                                  StreamTag>,
                               std::unique_ptr<Modifier>,
                               Tuple>> {
        auto modifier = std::make_unique<Modifier>(std::forward<Modifier>(_modifier));
        auto newRange = modifier->modify(begin, end);

        return Stream<Container,
                      Generator,
                      typename Modifier::template Accessor<Accessor>,
                      std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                        typename Modifier::StreamTag>,
                                         FiniteStreamTag,
                                         StreamTag>,
                      std::unique_ptr<Modifier>,
                      Tuple>(std::move(container),
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

template <class Iterator>
Stream(Iterator, Iterator,
       std::enable_if_t<isIterator<Iterator>, BadType> = BAD_TYPE_INSTANCE)
-> Stream<BadType,
          BadType,
          IterAccessor<decltype(*std::declval<Iterator>()), Iterator>,
          FiniteStreamTag>;

template <class Container>
explicit Stream(const Container &,
                std::enable_if_t<isContainer<Container>, BadType> = BAD_TYPE_INSTANCE)
-> Stream<Container,
          BadType,
          IterAccessor<typename Container::value_type, typename Container::const_iterator>,
          FiniteStreamTag>;

template <class Container>
explicit Stream(const Container &&,
                std::enable_if_t<isContainer<Container>, BadType> = BAD_TYPE_INSTANCE)
-> Stream<Container,
          BadType,
          IterAccessor<typename Container::value_type &, typename Container::iterator>,
          FiniteStreamTag>;

template <class T>
explicit Stream(std::initializer_list<T>)
-> Stream<std::vector<T>,
          BadType,
          IterAccessor<T &, typename std::vector<T>::iterator>,
          FiniteStreamTag>;

template <class Generator>
explicit Stream(const Generator &,
                std::enable_if_t<isGenerator<Generator>, BadType> = BAD_TYPE_INSTANCE)
-> Stream<BadType,
          Generator,
          GenAccessor<Generator>,
          InfiniteStreamTag>;

template <class Generator>
explicit Stream(const Generator &&,
                std::enable_if_t<isGenerator<Generator>, BadType> = BAD_TYPE_INSTANCE)
-> Stream<BadType,
          Generator,
          GenAccessor<Generator>,
          InfiniteStreamTag>;

template <class T, class... Vs>
explicit Stream(T &&, Vs &&...)
-> Stream<std::vector<std::enable_if_t<isType<T>, T>>,
          BadType,
          IterAccessor<T &, typename std::vector<T>::iterator>,
          FiniteStreamTag>;

template <class T>
explicit Stream(T &&)
-> Stream<std::vector<std::enable_if_t<isType<T>, T>>,
          BadType,
          IterAccessor<T &, typename std::vector<T>::iterator>,
          FiniteStreamTag>;

#endif //STREAM_STREAM_H
