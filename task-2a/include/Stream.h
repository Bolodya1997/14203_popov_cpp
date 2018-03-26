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

template <class T,
          class Container,
          class Generator,
          class Accessor,
          class StreamTag,
          class... Modifiers>
class Stream;

template <class T,
          class Container,
          class Generator,
          class Accessor,
          class StreamTag,
          class NewModifier,
          class... OldModifiers>
class Stream<T,
             Container,
             Generator,
             Accessor,
             StreamTag,
             std::unique_ptr<NewModifier>, std::tuple<OldModifiers...>>;

template <class T,
          class Container,
          class Generator,
          class Accessor,
          class StreamTag,
          class... Modifiers>
class Stream {
public:
    template <class Iterator>
    Stream(Iterator _begin, Iterator _end,
           std::enable_if_t<isIterator<Iterator>, BadType> = BAD_TYPE_INSTANCE)
            : begin(IterAccessor<T, Iterator>(_begin)),
              end(IterAccessor<T, Iterator>(_end)) {
    }

    explicit Stream(const Container & _container,
                    std::enable_if_t<isContainer<Container>, BadType> = BAD_TYPE_INSTANCE)
            : begin(IterAccessor<T, typename Container::const_iterator>(_container.begin())),
              end(IterAccessor<T, typename Container::const_iterator>(_container.end())) {
    }

    explicit Stream(Container && _container,
                    std::enable_if_t<isContainer<Container>, BadType> = BAD_TYPE_INSTANCE)
            : container(std::move(_container)),
              begin(IterAccessor<T, typename Container::const_iterator>(container.begin())),
              end(IterAccessor<T, typename Container::const_iterator>(container.end())) {
    }

    explicit Stream(std::initializer_list<T> init)
            : container(std::move(Container(init))),
              begin(IterAccessor<T, typename Container::const_iterator>(container.begin())),
              end(IterAccessor<T, typename Container::const_iterator>(container.end())) {
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

    template <class... Vs>
    explicit Stream(const T & value, Vs &&... values)
            : Stream{ value, std::forward<Vs>(values)... } {
    }

//    template <class... Vs>
//    explicit Stream(T && value, Vs &&... values)
//            : Stream{ std::move(value), std::forward<Vs>(values)... } {
//    }

    Stream(const Stream &) = delete;
    Stream(Stream &&) noexcept = default;

    ~Stream() = default;

    template <class Modifier>
    auto operator>>(Modifier && _modifier)
    -> std::enable_if_t<isModifier<Modifier, Accessor>,
                        Stream<typename Modifier::Accessor<Accessor>::Type,
                               Container,
                               Generator,
                               typename Modifier::Accessor<Accessor>,
                               std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                                 typename Modifier::StreamTag>,
                                                  FiniteStreamTag,
                                                  StreamTag>,
                               std::unique_ptr<Modifier>, std::tuple<Modifiers...>>> {
        auto modifier = std::make_unique<Modifier>(std::forward<Modifier>(_modifier));
        auto newRange = modifier->modify(begin, end);

        return Stream<typename Modifier::Accessor<Accessor>::Type,
                      Container,
                      Generator,
                      typename Modifier::Accessor<Accessor>,
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
    Stream(Stream &&) noexcept = default;

    ~Stream() = default;

    template <class Modifier>
    auto operator>>(Modifier && _modifier)
    -> std::enable_if_t<isModifier<Modifier, Accessor>,
            Stream<typename Modifier::Accessor::Type,
                   Container,
                   Generator,
                   typename Modifier::Accessor,
                   std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                     typename Modifier::StreamTag>,
                                      FiniteStreamTag,
                                      StreamTag>,
                   std::unique_ptr<Modifier>, Tuple>> {
        auto modifier = std::make_unique<Modifier>(std::forward<Modifier>(_modifier));
        auto newRange = modifier->modify(begin, end);

        return Stream<typename Modifier::Accessor<Accessor>::Type,
                      Container,
                      Generator,
                      typename Modifier::Accessor<Accessor>,
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

template <class Iterator>
Stream(Iterator, Iterator,
       std::enable_if_t<isIterator<Iterator>, BadType> = BAD_TYPE_INSTANCE)
-> Stream<decltype(*std::declval<Iterator>()),
          BadType,
          BadType,
          IterAccessor<decltype(*std::declval<Iterator>()), Iterator>,
          FiniteStreamTag>;

template <class Container>
explicit Stream(const Container &,
                std::enable_if_t<isContainer<Container>, BadType> = BAD_TYPE_INSTANCE)
-> Stream<typename Container::value_type,
          BadType,
          BadType,
          IterAccessor<typename Container::value_type, typename Container::const_iterator>,
          FiniteStreamTag>;

template <class Container>
explicit Stream(Container &&,
                std::enable_if_t<isContainer<Container>, BadType> = BAD_TYPE_INSTANCE)
-> Stream<typename Container::value_type &,
          Container,
          BadType,
          IterAccessor<typename Container::value_type &, typename Container::const_iterator>,
          FiniteStreamTag>;

template <class T>
explicit Stream(std::initializer_list<T>)
-> Stream<T &,
          std::vector<T>,
          BadType,
          IterAccessor<T &, typename std::vector<T>::const_iterator>,
          FiniteStreamTag>;

template <class Generator>
explicit Stream(const Generator &,
                std::enable_if_t<isGenerator<Generator>, BadType> = BAD_TYPE_INSTANCE)
-> Stream<decltype(std::declval<Generator>()()),
          BadType,
          BadType,
          GenAccessor<Generator>,
          InfiniteStreamTag>;

template <class Generator>
explicit Stream(Generator &&,
                std::enable_if_t<isGenerator<Generator>, BadType> = BAD_TYPE_INSTANCE)
-> Stream<decltype(std::declval<Generator>()()),
          BadType,
          Generator,
          GenAccessor<Generator>,
          InfiniteStreamTag>;

template <class T, class... Vs>
explicit Stream(const T &, Vs &&...)
-> Stream<T,
          std::vector<T>,
          BadType,
          IterAccessor<T, typename std::vector<T>::const_iterator>,
          FiniteStreamTag>;

//template <class T, class... Vs>
//explicit Stream(T &&, Vs &&...)
//-> Stream<T &,
//          std::vector<T>,
//          BadType,
//          IterAccessor<T &, typename std::vector<T>::const_iterator>,
//          FiniteStreamTag>;


#endif //STREAM_STREAM_H
