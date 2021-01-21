#pragma once
#include <vector>

#include "types.hpp"
namespace ExplodingKittens {

class ICardPool {
   public:
    virtual void PutBackBomb(int pos) = 0;
    virtual CardType Front() = 0;
    virtual CardType Back() = 0;
    virtual void PopFront() = 0;
    virtual void PopBack() = 0;
    virtual void ShuffleCards() = 0;
    virtual std::vector<std::vector<CardType>> InitializePlayerCards() = 0;
    virtual void PrintCardPool() = 0;
};
}  // namespace ExplodingKittens