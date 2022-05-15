#include "tree_interval.h"

int main()
{
    std::vector<int> arr = {7, -2, 10, 0, -1, 6, 11, -8, 2, -1};
    tree_interval<int> a(arr.begin(), arr.end());

    for (size_t i = 0; i < a.size(); ++i)
    {
        for (size_t j = i + 1; j < a.size(); ++j)
        {
            std::cout << a.get_sum_interval(i, j) << "  ";
        }
        std::cout << '\n';
    }

    int n = 20;
    int *arr2 = get_random_array(n);
    for (size_t i = 0; i < n; ++i)
    {
        std::cout << arr2[i] << " ";
    }
    std::cout << '\n';
    return 0;
}
