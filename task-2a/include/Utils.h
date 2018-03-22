#ifndef STREAM_UTILS_H
#define STREAM_UTILS_H

struct BadType {
    using const_iterator = BadType;

    using difference_type = BadType;
    using value_type = BadType;
    using pointer = BadType;
    using reference = BadType;
    using iterator_category = BadType;
};

static const BadType BAD_TYPE_INSTANCE = BadType();

struct InfiniteStreamTag {
};
struct FiniteStreamTag {
};

#endif //STREAM_UTILS_H
