#ifndef TREE_INTERVAL_H
#define TREE_INTERVAL_H
#include <iostream>
#include <stack>
#include <tuple>
#include <vector>
#include "random.h"

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

        int &get_left_border() noexcept { return interval.l; }
        int &get_right_border() noexcept { return interval.r; }
    };

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

public:
    template <typename _Iter>
    tree_interval(_Iter __first, _Iter __last) : sz(static_cast<size_t>(std::distance(__first, __last)))
    {
        size_t l = 0;
        size_t r = sz;

        root = new __node;
        root->val = __sum_init(l, r, __first);
        root->interval = {l, r};
        root->prev = nullptr;
        root->right = nullptr;
        root->left = nullptr;

        __node *tail = root;
        // std::cout << tail->get_left_border() << "  " << tail->get_right_border() << "  " << root->val << std::endl;
        while (tail != nullptr)
        {
            l = tail->interval.l;
            r = tail->interval.r;
            if (tail->left == nullptr)
            {
                while (tail->get_left_border() < tail->get_right_border() - 1)
                {
                    int m = (l + r) >> 1;
                    __node *elem = new __node;
                    elem->val = __sum_init(l, m, __first);
                    elem->get_left_border() = l;
                    elem->get_right_border() = m;
                    elem->prev = tail;
                    elem->left = nullptr;
                    elem->right = nullptr;

                    tail->left = elem;
                    tail = tail->left;
                    r = m + 1;
                    //        std::cout << "left: " << tail->get_left_border() << "  " << tail->get_right_border() << "  " << tail->val << std::endl;
                }
            }

            tail = tail->prev;

            if (tail == nullptr)
                break;

            l = tail->left->get_right_border();
            r = tail->get_right_border();
            if (tail->right == nullptr)
            {
                __node *elem = new __node;
                elem->val = tail->val - tail->left->val;
                elem->interval = {l, r};
                elem->prev = tail;
                elem->left = nullptr;
                elem->right = nullptr;
                tail->right = elem;
                tail = tail->right;
                //      std::cout << "right: " << tail->get_left_border() << "  " << tail->get_right_border() << "  " << tail->val << std::endl;
            }
        }
        // std::cout << "-----------\n";
    }

    _Ty get_sum_interval(int left_border, int right_border)
    {
        std::stack<std::tuple<__interval, __node *>> st;
        _Ty sum = _Ty(); // результат
        __node *tail = root;
        st.emplace(__interval{left_border, right_border}, tail);
        while (!st.empty())
        {

            __interval tmp_interval = std::get<0>(st.top());
            tail = std::get<1>(st.top());
            st.pop();

            if (tmp_interval.l == tail->get_left_border() && tmp_interval.r == tail->get_right_border())
            {
                //  std::cout << "eq: " << sum << "  " << tmp_interval.l << "  " << tmp_interval.r << "  " << tail->val << std::endl;
                sum += tail->val;
                if (!st.empty())
                {
                    tail = std::get<1>(st.top());
                    tmp_interval = std::get<0>(st.top());
                    st.pop();
                }
                else
                {
                    break;
                }
            }
            if (tail->right != nullptr && tmp_interval.r > tail->right->get_left_border())
            {

                if (tail->right != nullptr)
                {
                    // std::cout << "right: " << sum << "  " << tail->right->get_left_border() << "  " << tmp_interval.r << "  " << tail->val << std::endl;
                    st.emplace(__interval{tail->right->get_left_border(), tmp_interval.r}, tail->right);
                }
                else
                {
                    break;
                }
            }
            if (tail->right != nullptr && !st.empty() && std::get<0>(st.top()).l == tail->right->get_left_border() && std::get<0>(st.top()).r == tail->right->get_right_border())
            {
                // std::cout << "eq: " << sum << "  " << std::get<0>(st.top()).l << "  " << std::get<0>(st.top()).r << "  " << tail->right->val << std::endl;
                sum += tail->right->val;
                st.pop();
            }
            if (tail->left != nullptr && tmp_interval.l < tail->left->get_right_border())
            {

                if (tail->left != nullptr)
                {
                    // std::cout << "left: " << sum << "  " << tmp_interval.l << "  " << std::min(tail->left->get_right_border(), tmp_interval.r) << "  " << tail->val << std::endl;
                    st.emplace(__interval{tmp_interval.l, std::min(tmp_interval.r, tail->left->get_right_border())}, tail->left);
                }
                else
                {
                    break;
                }
            }
        }

        return sum;
    }

    size_t size() const noexcept { return sz; }
};

#endif
