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

        constexpr inline const size_t &get_left_border() const noexcept { return interval.l; }
        constexpr inline const size_t &get_right_border() const noexcept { return interval.r; }
    };

    using reb_alloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<__node>;
    using alloc_traits = std::allocator_traits<reb_alloc>;

    reb_alloc alloc;
    __node *root;
    size_t sz;
    std::stack<std::tuple<__interval, __node *>> st;

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

    inline bool __check_border_eq(const __node *__ptr, const __interval &__intrvl) noexcept
    {
        return (__ptr->get_left_border() == __intrvl.l) && (__ptr->get_right_border() == __intrvl.r);
    }

    constexpr inline const _Ty &__get_val(const __node *&__ptr) const noexcept { return __ptr->val; }

    constexpr inline __node *__left(const __node *__ptr) { return __ptr->left; }

    constexpr inline __node *__right(const __node *__ptr) { return __ptr->right; }

    inline _Ty __procedure_get_sum(size_t l, size_t r, const __node *tail, size_t tl, size_t tr)
    {
        if (!tail)
            return 0;

        if (l <= tl && tr <= r)
        {
            return __get_val(tail);
        }

        if (tr < l || r < tl)
        {
            return 0;
        }

        size_t tm = (tl + tr) >> 1;
        return __procedure_get_sum(l, r, tail->left, tl, tm) + __procedure_get_sum(l, r, tail->right, tm, tr);
    }

public:
    template <typename _Iter>
    tree_interval(_Iter __first, _Iter __last) : alloc(), sz(static_cast<size_t>(std::distance(__first, __last))), st()
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
    /*
    Запрос суммы
l, r - границы запроса;
v - номер текущей вершины; tl, tr - границы соответствующего отрезка
int get_sum(int l, int r, int v, int tl, int tr) {
    вариант 1
    if (l <= tl && tr <= r) {
        return tree[v];
    }

    вариант 2
    if (tr < l || r < tl) {
        return 0;
    }

    вариант 3
    int tm = (tl + tr) / 2;
    return get_sum(l, r, v * 2,     tl,     tm)
         + get_sum(l, r, v * 2 + 1, tm + 1, tr);
}
    */

    _Ty get_sum(const size_t &left_border, const size_t &right_border)
    {
        return __procedure_get_sum(left_border, right_border, root, root->get_left_border(), root->get_right_border());
    }

    _Ty get_sum_interval(size_t left_border, size_t right_border)
    {
        _Ty sum = _Ty(); // результат
        __node *tail = root;
        st.emplace(__interval{left_border, right_border}, tail);
        __interval tmp_interval;
        while (!st.empty())
        {
            tmp_interval = std::get<0>(st.top());
            tail = std::get<1>(st.top());
            st.pop();
            tail = tail->right;
            if (tmp_interval.r > tail->get_left_border())
            {
                // std::cout << "right: " << std::max(tail->get_left_border(), tmp_interval.l) << "  " << tmp_interval.r << "  sum = " << sum << std::endl;

                std::tuple p = std::make_tuple(__interval{std::max(tail->get_left_border(), tmp_interval.l), tmp_interval.r}, tail);
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
            if (tmp_interval.l < tail->get_right_border())
            {
                // std::cout << "left: " << tmp_interval.l << "  " << std::min(tmp_interval.r, tail->get_right_border()) << "  sum = " << sum << std::endl;

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
