#ifndef ROOT_DECOMPOSITION_H
#define ROOT_DECOMPOSITION_H
#include <iostream>
#include <math.h>
template <typename _Ty>
class root_decomposition
{
protected:
    size_t sz_ptr;
    size_t sz_ptr_sum;
    _Ty *__ptr;
    _Ty *__ptr_sum;

    template <typename _Iter>
    void __fill_ptr(_Iter __first, _Iter __last)
    {
        _Ty *__tail_ptr = __ptr;
        while (__first != __last)
        {
            *__tail_ptr = *__first;
            ++__tail_ptr;
            ++__first;
        }
    }

    void __fill_ptr_sum(_Ty *tail_ptr, _Ty *last_ptr, _Ty *tail_ptr_sum)
    {
        size_t border = sqrt(sz_ptr) + 1;

        while (tail_ptr != last_ptr)
        {
            size_t l = 0;
            _Ty sum = _Ty();
            while (l < border)
            {
                sum += *tail_ptr;
                ++tail_ptr;
                ++l;
                if (tail_ptr == last_ptr)
                {
                    break;
                }
            }
            *tail_ptr_sum = sum;
            ++tail_ptr_sum;
        }
    }

public:
    template <typename _Iter>
    root_decomposition(_Iter __first, _Iter __last) : sz_ptr(std::distance(__first, __last)),
                                                      sz_ptr_sum(sqrt(sz_ptr) + 1),
                                                      __ptr(reinterpret_cast<_Ty *>(::operator new(sz_ptr * sizeof(_Ty)))),
                                                      __ptr_sum(reinterpret_cast<_Ty *>(::operator new(sz_ptr * sizeof(_Ty))))
    {
        __fill_ptr(__first, __last);
        __fill_ptr_sum(__ptr, __ptr + sz_ptr, __ptr_sum);
    }

    ~root_decomposition()
    {
        ::operator delete(__ptr);
        ::operator delete(__ptr_sum);
    }

    void change_value(size_t key, _Ty value)
    {
        int tmp_val = __ptr[key];
        __ptr[key] = value;
        __ptr_sum[key / static_cast<size_t>(sqrt(sz_ptr_sum))] -= tmp_val;
        __ptr_sum[key / static_cast<size_t>(sqrt(sz_ptr_sum))] += value;
    }

    _Ty get_sum_interval(size_t l, size_t r)
    {
        --r;
        _Ty sum = _Ty();
        size_t c_l = l / sz_ptr_sum, c_r = r / sz_ptr_sum;
        if (c_l == c_r)
        {
            for (size_t i = l; i <= r; ++i)
            {
                sum += __ptr[i];
            }
        }
        else
        {
            for (size_t i = l, end = (c_l + 1) * sz_ptr_sum - 1; i <= end; ++i)
            {
                sum += __ptr[i];
            }
            for (size_t i = c_l + 1; i <= c_r - 1; ++i)
            {
                sum += __ptr_sum[i];
            }
            for (size_t i = c_r * sz_ptr_sum; i <= r; ++i)
            {
                sum += __ptr[i];
            }
        }
        return sum;
    }

    void print()
    {
        for (size_t i = 0; i < sz_ptr_sum; ++i)
        {
            std::cout << __ptr_sum[i] << "  ";
        }
        std::cout << '\n';
    }

    size_t size() const noexcept { return sz_ptr; }
};

#endif