#ifndef STCK_H
#define STCK_H

#include "li.h"

template <typename _Ty, typename _Sequence = li<_Ty>>
class stack
{
protected:
    _Sequence seq;

public:
    stack() : seq() {}

    template <typename... Args>
    void emplace(Args &&...args) { seq.emplace_back(args...); }

    _Ty &top() { return seq.back(); }

    bool empty() noexcept { return seq.empty(); }

    void pop() { seq.pop_back(); }

    ~stack() {}
};

#endif