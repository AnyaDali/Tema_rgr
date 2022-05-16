#include "stck.h"

int main()
{

    stack<int> p;
    p.emplace(10);
    p.emplace(20);
    p.emplace(30);
    std::cout << p.top() << std::endl;
    p.pop();
    std::cout << p.empty() << std::endl;
    return 0;
}