#pragma once
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "interface.hpp"
namespace ExplodingKittens {

#define BombNum 4
#define SkipNum 8
#define ShirkNum 5
#define ReverseNum 5
#define PredictNum 4
#define SeeThroughNum 4
#define SwapNum 3
#define GetBottomNum 3
#define ShuffleNum 4
#define ExtortNum 4
#define BombDisposalNum 6
#define InitialCardsNum 5

class CardPool : public ICardPool {
   public:
    CardPool() {}
    ~CardPool() {}
    virtual void PutBackBomb(int pos) override;
    virtual CardType Front() override;
    virtual CardType Back() override;
    virtual void PopFront() override;
    virtual void PopBack() override;
    virtual std::vector<CardType> SeeThroughCards() override;
    virtual int PredictBombPos() override;
    virtual void ShuffleCards() override;
    virtual std::vector<std::vector<CardType>> InitializePlayerCards() override;
    virtual void PrintCardPool() override;
    static const int PredictCardNum = 3;

   private:
    std::vector<CardType> mCards;
};
}  // namespace ExplodingKittens