#include "tree_interval.h"
#include "root_decomposition.h"

const int N = 1000;

int main()
{
    //   int *arr = get_random_array(N);
    std::vector<int> arr = {7, -2, 10, 0, -1, 6, 11, -8, 2, -1};
    auto t1 = clock();
    tree_interval<int> a(arr.begin(), arr.end());

    for (size_t i = 0; i < a.size(); ++i)
    {
        for (size_t j = i + 1; j < a.size(); ++j)
        {
            std::cout << a.get_sum_interval(i, j) << "  ";
        }
        std::cout << '\n';
    }
    auto t2 = clock();
    std::cout << static_cast<double>(t2 - t1) / CLOCKS_PER_SEC << std::endl;

    t1 = clock();
    root_decomposition<int> b(arr.begin(), arr.end());
    for (size_t i = 0; i < b.size(); ++i)
    {
        for (size_t j = i + 1; j < b.size(); ++j)
        {
            std::cout << b.get_sum_interval(i, j) << "  ";
        }
        std::cout << '\n';
    }
    std::cout << "+\n";
    t2 = clock();
    std::cout << static_cast<double>(t2 - t1) / CLOCKS_PER_SEC << std::endl;
    return 0;
}
