#ifndef STREAM_FILTER_H
#define STREAM_FILTER_H

#include <iterator>
#include <memory>

#include "Utils.h"

template <class Predicate>
class filter {
public:
    using StreamTag = InfiniteStreamTag;

    template <class SAccessor>
    class Accessor {
        using NoRefType = std::remove_reference_t<typename SAccessor::Type>;

    public:
        using Type = typename SAccessor::Type;

        Accessor() = delete;

        Accessor(const SAccessor & _sAccessor,
                 Predicate & _predicate)
                : sAccessor(_sAccessor),
                  predicate(_predicate) {
        }

        Accessor(const Accessor & other)
                : sAccessor(other.sAccessor),
                  predicate(other.predicate) {
        }

        ~Accessor() {
            if constexpr (!std::is_reference_v<Type>)
                delete valuePtr;
            valuePtr = nullptr;
        };

        Accessor & operator=(const Accessor &) = delete;

        bool operator==(const Accessor & other) const {
            return sAccessor == other.sAccessor;
        }

        bool operator!=(const Accessor & other) const {
            return sAccessor != other.sAccessor;
        }

        bool hasValue() {
            if (!sAccessor.hasValue())
                return false;

            if (valuePtr == nullptr) {
                if constexpr (std::is_reference_v<Type>)
                    valuePtr = reinterpret_cast<NoRefType *>(&(*sAccessor));
                else
                    valuePtr = reinterpret_cast<NoRefType *>(new Type(std::move(*sAccessor)));
            }

            return predicate(*valuePtr);
        }

        Type operator*() {
            if (!hasValue())
                throw std::runtime_error("filter::Accessor has no value");

            if constexpr (std::is_reference_v<Type>)
                return reinterpret_cast<Type>(*valuePtr);
            else
                return std::move(*reinterpret_cast<Type *>(valuePtr));
        }

        Accessor & operator++() {
            ++sAccessor;

            if constexpr (!std::is_reference_v<Type>)
                delete valuePtr;
            valuePtr = nullptr;

            return *this;
        }

    private:
        SAccessor sAccessor;

        Predicate & predicate;

        NoRefType * valuePtr = nullptr;
    };

    filter() = delete;

    explicit filter(Predicate & _predicate)
            : predicate(_predicate) {
    }

    explicit filter(Predicate && _predicate)
            : predicate(std::move(_predicate)) {
    }

    filter(const filter &) = default;
    filter(filter &&) noexcept = default;

    template <class SAccessor>
    auto modify(SAccessor begin, SAccessor end) {
        return std::pair{ Accessor<SAccessor>(begin, predicate),
                          Accessor<SAccessor>(end, predicate) };
    }

private:
    Predicate predicate;
};

#endif //STREAM_FILTER_H
