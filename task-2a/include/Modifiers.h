#ifndef STREAM_MODIFIERS_H
#define STREAM_MODIFIERS_H

class get {     //  TODO: get
};

template <class F>
class map {
public:
    explicit map(const F & _f)
            : f(_f) {
    }

    template <class In>
    auto modify(const In & value) const {
        return f(value);
    }

private:
    F f;
};

class filter {  //  TODO: filter
};

class skip {    //  TODO: skip
};

class group {   //  TODO: group
};

#endif //STREAM_MODIFIERS_H
