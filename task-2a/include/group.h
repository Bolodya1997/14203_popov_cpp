#ifndef STREAM_GROUP_H
#define STREAM_GROUP_H

#include <vector>
#include <stdexcept>

#include "Utils.h"

class group {
public:
    using StreamTag = InfiniteStreamTag;

    template <class SAccessor>
    class Accessor {
        using SuperType = std::remove_reference<typename SAccessor::Type>;

    public:
        using Type = std::vector<SuperType>;

        Accessor() = delete;

        Accessor(const SAccessor & _sAccessor, const SAccessor & _end,
                 std::size_t _n)
                : sAccessor(_sAccessor),
                  end(_end),
                  n(_n) {
            if (n == 0)
                throw std::invalid_argument("n must be > 0");
        }

        Accessor(const Accessor &) = default;
        Accessor(Accessor &&) noexcept = default;

        ~Accessor() = default;

        Accessor & operator=(const Accessor &) = delete;
        Accessor & operator=(Accessor &&) noexcept = delete;

        bool operator==(const Accessor & other) const {
            return sAccessor == other.sAccessor;
        }

        bool operator!=(const Accessor & other) const {
            return sAccessor != other.sAccessor;
        }

        bool hasValue() const {
            return true;
        }

        Type operator*() {
            if (dereferenced)
                throw std::runtime_error("double dereferencing group::Accessor");

            auto res = std::vector<SuperType>();

            for (std::size_t i = 0; i < n && sAccessor != end; ++sAccessor) {
                if (sAccessor.hasValue()) {
                    res.push_back(*sAccessor);

                    ++i;
                }
            }

            dereferenced = true;

            return res;
        }

        Accessor & operator++() {
            if (!dereferenced) {
                for (std::size_t i = 0; i < n && sAccessor != end; ++sAccessor) {
                    if (sAccessor.hasValue())
                        ++i;
                }
            }
            dereferenced = false;

            return *this;
        }

    private:
        SAccessor sAccessor;
        const SAccessor end;

        const std::size_t n;

        bool dereferenced = false;
    };

    group() = delete;
    explicit group(const std::size_t & _n)
            : n(_n) {
    }

    template <class SAccessor>
    auto modify(SAccessor begin, SAccessor end) const {
        return std::pair{ Accessor(begin, end, n),
                          Accessor(end, end, n) };
    }

private:
    const std::size_t n;
};

#endif //STREAM_GROUP_H
