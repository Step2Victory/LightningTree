#pragma once
#include <iterator>
#include <random>
#include <vector>
#include <array>
#include <utility>
#include <algorithm>
#include "../Vector.h"
#include "../Vertex.h"


class MaxPotentialEdgeIterator
{
   std::array<std::pair<double, std::array<int, 3>>, 26>* res_;
   size_t i_;
public:
    MaxPotentialEdgeIterator(std::array<std::pair<double, std::array<int, 3>>, 26>* res, size_t i) : res_(res), i_(i)
    {
        
    }
    std::array<int, 3> operator*()
    {
        return (*res_)[i_].second;
    }
    MaxPotentialEdgeIterator operator++()
    {
        i_++;
        return *this;
    }

    bool operator!=(const MaxPotentialEdgeIterator& rhs) const
    {
        return i_ != rhs.i_;
    }
};

class MaxPotentialEdgeRange
{
    std::array<std::pair<double, std::array<int, 3>>, 26> res_; 
public:
    MaxPotentialEdgeRange(std::array<std::pair<double, std::array<int, 3>>, 26> res) : res_(res)
    {
        auto cmp = [](const std::pair<double, std::array<int, 3>>& lhs, std::pair<double, std::array<int, 3>>& rhs){
            return lhs.first > rhs.first;
        };
        std::sort(res_.begin(), res_.end(), cmp);
    };
    MaxPotentialEdgeIterator begin()
    {
        return MaxPotentialEdgeIterator{&res_, 0};
    }

    MaxPotentialEdgeIterator end()
    {
        return MaxPotentialEdgeIterator{&res_, 26};
    }

};


inline MaxPotentialEdgeRange MaxPotenialPoint(std::array<std::pair<double, std::array<int, 3>>, 26> res)
{
    return MaxPotentialEdgeRange(res);
}