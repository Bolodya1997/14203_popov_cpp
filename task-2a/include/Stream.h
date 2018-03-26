#ifndef STREAM_STREAM_H
#define STREAM_STREAM_H

#include <vector>
#include <tuple>
#include <memory>
#include <type_traits>

#include <iostream>

#include "Utils.h"
#include "TypeTraits.h"
#include "IterAccessor.h"
#include "GenAccessor.h"

struct BadType {};
struct BadContainer {};
struct BadGenerator {};

struct BaseSpecialization {};
struct IteratorSpecialization {};
struct ContainerSpecialization {};
struct InitializerListSpecialization {};
struct GeneratorSpecialization {};
struct PackSpecialization {};

template <class StreamSpecialization,
          class Container,
          class Generator,
          class Accessor,
          class StreamTag,
          class... Modifiers>
class Stream {
public:
    Stream() = delete;

    Stream(const Stream &) = delete;
    Stream(Stream &&) noexcept = delete;

    virtual ~Stream() = default;

    template <class Modifier>
    auto operator>>(Modifier && _modifier)
    -> std::enable_if_t<isModifier<Modifier, Accessor>,
                        Stream<BaseSpecialization,
                               Container,
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

        return Stream<BaseSpecialization,
                      Container,
                      Generator,
                      typename Modifier::template Accessor<Accessor>,
                      std::conditional_t<std::is_same_v<FiniteStreamTag,
                                                        typename Modifier::StreamTag>,
                                         FiniteStreamTag,
                                         StreamTag>,
                      std::unique_ptr<Modifier>,
                      std::tuple<Modifiers...>>(std::move(container),
                                                std::move(generatorPtr),
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
    explicit Stream(Container && _container,
                    std::unique_ptr<Generator> && _generatorPtr,
                    Accessor _begin,
                    Accessor _end,
                    Modifiers &&... _modifiers)
            : container(std::move(_container)),
              generatorPtr(std::move(_generatorPtr)),
              begin(_begin),
              end(_end),
              modifiers(std::move(_modifiers)...) {
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    explicit Stream(Container && _container)
            : container(std::move(_container)),
              generatorPtr(nullptr),
              begin(Accessor(container.begin())),
              end(Accessor(container.end())),
              modifiers() {
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    explicit Stream(Generator && _generator)
            : container(BadContainer()),
              generatorPtr(std::make_unique<Generator>(std::move(_generator))),
              begin(Accessor(*generatorPtr)),
              end(Accessor(*generatorPtr)),
              modifiers() {
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    Container container;
    std::unique_ptr<Generator> generatorPtr;

    Accessor begin;
    Accessor end;

    std::tuple<Modifiers...> modifiers;

    template <class, class, class, class, class, class...>
    friend class Stream;
};

template <class Container,
          class Generator,
          class Accessor,
          class StreamTag>
class Stream<IteratorSpecialization,
             Container,
             Generator,
             Accessor,
             StreamTag> : public Stream<BaseSpecialization,
                                        Container,
                                        Generator,
                                        Accessor,
                                        StreamTag> {
    using Super = Stream<BaseSpecialization,
                         Container,
                         Generator,
                         Accessor,
                         StreamTag>;
public:
    template <class Iterator>
    Stream(Iterator begin, Iterator end,
           BadType * = nullptr)
            : Super(Container(),
                    nullptr,
                    Accessor(begin),
                    Accessor(end)) {
//        std::cout << "iterator" << std::endl;
    }
};

template <class Container,
          class Generator,
          class Accessor,
          class StreamTag>
class Stream<ContainerSpecialization,
             Container,
             Generator,
             Accessor,
             StreamTag> : public Stream<BaseSpecialization,
                                        Container,
                                        Generator,
                                        Accessor,
                                        StreamTag> {
    using Super = Stream<BaseSpecialization,
                         Container,
                         Generator,
                         Accessor,
                         StreamTag>;

public:
    explicit Stream(const Container & container,
                    BadType * = nullptr)
            : Super(Container(),
                    nullptr,
                    Accessor(container.begin()),
                    Accessor(container.end())) {
//        std::cout << "ref container" << std::endl;
    }

    explicit Stream(Container && container,
                    BadType * = nullptr)
            : Super(std::move(container)) {
//        std::cout << "move container" << std::endl;
    }
};

template <class Container,
          class Generator,
          class Accessor,
          class StreamTag>
class Stream<InitializerListSpecialization,
             Container,
             Generator,
             Accessor,
             StreamTag> : public Stream<BaseSpecialization,
                                        Container,
                                        Generator,
                                        Accessor,
                                        StreamTag> {
    using Super = Stream<BaseSpecialization,
                         Container,
                         Generator,
                         Accessor,
                         StreamTag>;
public:
    template <class T>
    Stream(const std::initializer_list<T> & init)
            : Super(std::move(Container(init))) {
//        std::cout << "initializer list" << std::endl;
    }
};

template <class Container,
          class Generator,
          class Accessor,
          class StreamTag>
class Stream<GeneratorSpecialization,
             Container,
             Generator,
             Accessor,
             StreamTag> : public Stream<BaseSpecialization,
                                        Container,
                                        Generator,
                                        Accessor,
                                        StreamTag> {
    using Super = Stream<BaseSpecialization,
                         Container,
                         Generator,
                         Accessor,
                         StreamTag>;
public:
    explicit Stream(const Generator & generator,
                    BadType * = nullptr)
            : Super(Container(),
                    nullptr,
                    Accessor(generator),
                    Accessor(generator)) {
//        std::cout << "ref generator" << std::endl;
    }

    explicit Stream(Generator && generator,
                    BadType * = nullptr)
            : Super(std::move(generator)) {
        std::cout << "move generator" << std::endl;
    };
};

template <class Container,
          class Generator,
          class Accessor,
          class StreamTag>
class Stream<PackSpecialization,
             Container,
             Generator,
             Accessor,
             StreamTag> : public Stream<BaseSpecialization,
                                        Container,
                                        Generator,
                                        Accessor,
                                        StreamTag> {
    using Super = Stream<BaseSpecialization,
                         Container,
                         Generator,
                         Accessor,
                         StreamTag>;
public:
    template <class T, class... Vs>
    explicit Stream(T && value, Vs &&... values)
            : Super(std::move(makeContainer(std::forward<T>(value),
                                            std::forward<Vs>(values)...))) {
//        std::cout << "pack" << std::endl;
    }

private:
    template <class T, class... Vs>
    static Container makeContainer(T && value, Vs &&... values) {
        Container container;

        return makeContainer(std::move(container),
                             std::forward<T>(value), std::forward<Vs>(values)...);
    }

    template <class T, class... Vs>
    static Container makeContainer(Container && container,
                                   T && value, Vs &&... values) {
        container.push_back(std::forward<T>(value));

        return makeContainer(std::move(container), std::forward<Vs>(values)...);
    }

    template <class T>
    static Container makeContainer(Container && container,
                                   T && value) {
        container.push_back(std::forward<T>(value));

        return std::move(container);
    }
};

template <class Iterator>
Stream(Iterator, Iterator,
       std::enable_if_t<isIterator<Iterator>, BadType *> = nullptr)
-> Stream<IteratorSpecialization,
          BadContainer,
          BadGenerator,
          IterAccessor<std::conditional_t<isConstRef<decltype(*std::declval<Iterator>())>,
                                          removeCVRef<decltype(*std::declval<Iterator>())>,
                                          decltype(*std::declval<Iterator>())>,
                       Iterator>,
          FiniteStreamTag>;

template <class Container>
explicit Stream(const Container &,
                std::enable_if_t<isContainer<Container>, BadType *> = nullptr)
-> Stream<ContainerSpecialization,
          Container,
          BadGenerator,
          IterAccessor<typename Container::value_type, typename Container::const_iterator>,
          FiniteStreamTag>;

template <class Container>
explicit Stream(const Container &&,
                std::enable_if_t<isContainer<Container>, BadType *> = nullptr)
-> Stream<ContainerSpecialization,
          Container,
          BadGenerator,
          IterAccessor<typename Container::value_type &, typename Container::iterator>,
          FiniteStreamTag>;

template <class T>
Stream(const std::initializer_list<T> &)
-> Stream<InitializerListSpecialization,
          std::vector<T>,
          BadGenerator,
          IterAccessor<T &, typename std::vector<T>::iterator>,
          FiniteStreamTag>;

template <class T>
Stream(std::initializer_list<T> &&)
-> Stream<InitializerListSpecialization,
          std::vector<T>,
          BadGenerator,
          IterAccessor<T &, typename std::vector<T>::iterator>,
          FiniteStreamTag>;

template <class Generator>
explicit Stream(const Generator &,
                std::enable_if_t<isGenerator<Generator>, BadType *> = nullptr)
-> Stream<GeneratorSpecialization,
          BadContainer,
          Generator,
          GenAccessor<Generator>,
          InfiniteStreamTag>;

template <class Generator>
explicit Stream(const Generator &&,
                std::enable_if_t<isGenerator<Generator>, BadType *> = nullptr)
-> Stream<GeneratorSpecialization,
          BadContainer,
          Generator,
          GenAccessor<Generator>,
          InfiniteStreamTag>;

template <class T, class... Vs>
explicit Stream(T &&, Vs &&...)
-> Stream<PackSpecialization,
          std::enable_if_t<isType<T>,std::vector<std::remove_reference_t<T>>>,
          BadGenerator,
          IterAccessor<T &, typename std::vector<std::remove_reference_t<T>>::iterator>,
          FiniteStreamTag>;

template <class T>
explicit Stream(T &&)
-> Stream<PackSpecialization,
          std::enable_if_t<isType<T>, std::vector<std::remove_reference_t<T>>>,
          BadGenerator,
          IterAccessor<T &, typename std::vector<std::remove_reference_t<T>>::iterator>,
          FiniteStreamTag>;

#endif //STREAM_STREAM_H
