#ifndef TREE_INTERVAL_H
#define TREE_INTERVAL_H
#include <iostream>
#include <stack>
#include <tuple>
#include <vector>
#include "random.h"
#include "stck.h"
#include "pool_alloc.h"
#include <list>
template <typename _Ty, typename _Alloc = std::allocator<_Ty>>
class tree_interval
{
protected:
    struct __interval
    {
        size_t l, r;
    };

    struct __node
    {
        __interval interval;
        _Ty val;
        __node *left;
        __node *right;
        __node *prev;

        inline const size_t &get_left_border() const noexcept { return interval.l; }
        inline const size_t &get_right_border() const noexcept { return interval.r; }
    };

    using reb_alloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<__node>;
    using alloc_traits = std::allocator_traits<reb_alloc>;

    reb_alloc alloc;
    __node *root;
    size_t sz;

    template <typename _Iter>
    _Ty __sum_init(size_t l, size_t r, _Iter __ptr)
    {
        _Ty s = _Ty();
        __ptr += l;
        for (size_t i = l; i < r; ++i, ++__ptr)
        {
            s += *__ptr;
        }
        return s;
    }

    bool __check_border_eq(const __node *__ptr, const __interval &__intrvl) noexcept
    {
        return (__ptr->get_left_border() == __intrvl.l) && (__ptr->get_right_border() == __intrvl.r);
    }

public:
    template <typename _Iter>
    tree_interval(_Iter __first, _Iter __last) : sz(static_cast<size_t>(std::distance(__first, __last)))
    {
        size_t l = 0;
        size_t r = sz;

        root = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(
            alloc, root, __node{{l, r}, __sum_init(l, r, __first), nullptr, nullptr, nullptr});

        __node *tail = root;

        while (tail != nullptr)
        {
            l = tail->interval.l;
            r = tail->interval.r;
            if (tail->left == nullptr)
            {
                while (tail->get_left_border() < tail->get_right_border() - 1)
                {
                    size_t m = (l + r) >> 1;

                    __node *elem = alloc_traits::allocate(alloc, 1);
                    alloc_traits::construct(
                        alloc, elem, __node{{l, m}, __sum_init(l, m, __first), nullptr, nullptr, tail});

                    tail->left = elem;
                    tail = tail->left;
                    r = m + 1;
                }
            }

            tail = tail->prev;

            if (tail == nullptr)
                break;

            l = tail->left->get_right_border();
            r = tail->get_right_border();
            if (tail->right == nullptr)
            {
                __node *elem = alloc_traits::allocate(alloc, 1);
                alloc_traits::construct(
                    alloc, elem, __node{{l, r}, tail->val - tail->left->val, nullptr, nullptr, tail});

                tail->right = elem;
                tail = tail->right;
            }
        }
    }

    _Ty get_sum_interval(size_t left_border, size_t right_border)
    {
        std::stack<std::tuple<__interval, __node *> /*std::list<std::tuple<__interval, __node *>, pool_allocator<std::tuple<__interval, __node *>>> */> st;
        _Ty sum = _Ty(); // результат
        __node *tail = root;
        st.emplace(__interval{left_border, right_border}, tail);
        while (!st.empty())
        {
            __interval tmp_interval = std::get<0>(st.top());
            tail = std::get<1>(st.top());
            st.pop();
            tail = tail->right;
            if (tail != nullptr && tmp_interval.r > tail->get_left_border())
            {
                std::tuple p = std::make_tuple(__interval{std::max(tail->get_left_border(), tmp_interval.l), tmp_interval.r}, tail);
                // st.emplace(std::move(__interval{std::max(tail->right->get_left_border(), tmp_interval.l), tmp_interval.r}), tail->right);
                if (__check_border_eq(tail, std::get<0>(p)))
                {
                    sum += tail->val;
                }
                else
                {
                    st.emplace(std::move(p));
                }
            }
            tail = tail->prev;
            tail = tail->left;
            if (tail != nullptr && tmp_interval.l < tail->get_right_border())
            {
                std::tuple p = std::make_tuple(__interval{tmp_interval.l, std::min(tmp_interval.r, tail->get_right_border())}, tail);
                // st.emplace(std::move(__interval{tmp_interval.l, std::min(tmp_interval.r, tail->left->get_right_border())}), tail->left);
                if (__check_border_eq(tail, std::get<0>(p)))
                {
                    sum += tail->val;
                }
                else
                {
                    st.emplace(std::move(p));
                }
            }
        }

        return sum;
    }

    size_t size() const noexcept { return sz; }
};

#endif
