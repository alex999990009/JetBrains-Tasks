#include "SearcherEngine.hpp"

#include <iostream>

int main()
{
    try
    {
        searcher::SearcherEngine searcher;
        searcher.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
