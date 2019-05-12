
#include "Reference.hpp"
#include "Lambda.hpp"

// container
#include "Container/Optional.hpp"
#include "Container/Expected.hpp"

#include "Units/Quantity.hpp"

#include <iostream>

int main()
{
    rib::Optional<int> a(1);

    a.visit(VISITOR(x, x += 1;));

    std::cout << a.value_or(0) << " " << a.map([](int x) { return x + 1; }).value_or(0) << std::endl;

    return 0;
}
