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
        using SuperType = std::remove_reference_t<typename SAccessor::Type>;

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

        Accessor(const Accessor & other) = default;

        ~Accessor() = default;

        Accessor & operator=(const Accessor &) = delete;

        bool operator==(const Accessor & other) const {
            return sAccessor == other.sAccessor;
        }

        bool operator!=(const Accessor & other) const {
            return sAccessor != other.sAccessor;
        }

        bool hasValue() {
            return dereferenced || sAccessor.hasValue();
        }

        Type operator*() {
            if (dereferenced)
                throw std::runtime_error("double dereferencing group::Accessor");

            auto res = std::vector<SuperType>();

            for (std::size_t i = 0; i < n && sAccessor != end; ++sAccessor) {
                if (sAccessor.hasValue()) {
                    res.push_back(std::move(*sAccessor));

                    ++i;
                }
            }

            dereferenced = true;

            return std::move(res);
        }

        Accessor & operator++() {
            if (dereferenced) {
                dereferenced = false;
            } else if (!sAccessor.hasValue()) {
                ++sAccessor;
            } else {
                for (std::size_t i = 0; i < n && sAccessor != end; ++sAccessor) {
                    if (sAccessor.hasValue())
                        ++i;
                }
            }

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

    group(const group &) = default;
    group(group &&) noexcept = default;

    template <class SAccessor>
    auto modify(const SAccessor & begin, const SAccessor & end) const {
        return std::pair{ Accessor(begin, end, n),
                          Accessor(end, end, n) };
    }

private:
    const std::size_t n;
};

#endif //STREAM_GROUP_H
