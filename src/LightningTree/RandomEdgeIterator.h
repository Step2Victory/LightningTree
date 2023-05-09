#pragma once
#include <iterator>
#include <random>
#include <vector>
#include <array>
#include "../Vector.h"




// std::random_device rd_iter;
std::mt19937 gen_iter(42);
std::uniform_int_distribution<> distr(0, 26);




class RandomEdgeIterator
{
    std::array<std::array<std::array<bool, 3>, 3>, 3>* used;
    int counter;
    std::array<int,3> r;
    void Gen()
    {
        counter++;
        auto num = distr(gen_iter);
        int i = num % 3;
        num /= 3;
        int j = num % 3;
        num /= 3;
        int k = num % 3;
        while ((*used)[i][j][k])
        {
            num = distr(gen_iter);
            i = num % 3;
            num /= 3;
            j = num % 3;
            num /= 3;
            k = num % 3;
        }
        (*used)[i][j][k] = true;
        r = {i - 1, j - 1, k - 1};
    }
public:
    RandomEdgeIterator(std::array<std::array<std::array<bool, 3>, 3>, 3>* u, int counter_ = 0) : used(u), counter(counter_), r()
    {
        Gen();
    }
    std::array<int, 3> operator*()
    {
        return r;
    }
    RandomEdgeIterator operator++()
    {
        Gen();
        return *this;
    }

    bool operator!=(const RandomEdgeIterator& rhs) const
    {
        return counter != rhs.counter;
    }
};


class RandomPointGenerator
{
    std::array<std::array<std::array<bool, 3>, 3>, 3> used;
public:
    RandomPointGenerator()
    {
        for (size_t i = 0; i < 3; ++i)
        {
            for (size_t j = 0; j < 3; ++j)
            {
                for (size_t k = 0; k < 3; ++k)
                {
                    used[i][j][k] = false;
                }
            }
        }
    };
    RandomEdgeIterator begin()
    {
        return RandomEdgeIterator{&used, 0};
    }

    RandomEdgeIterator end()
    {
        return RandomEdgeIterator{&used, 25};
    }

};

inline RandomPointGenerator RandomPoint()
{
    return RandomPointGenerator{};
}