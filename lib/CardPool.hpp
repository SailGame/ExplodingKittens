#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <vector>
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
enum CardType {
    Bomb,
    Skip,
    Shirk,
    Reverse,
    Predict,
    SeeThrough,
    Swap,
    GetBottom,
    Shuffle,
    Extort,
    BombDisposal
};
static std::vector<std::string> CardNames = {
    "Bomb", "Skip",      "Shirk",   "Reverse", "Predict",     "SeeThrough",
    "Swap", "GetBottom", "Shuffle", "Extort",  "BombDisposal"};
class ICardPool {
public: 
    virtual void PutBackBomb(int pos) = 0;
    virtual CardType DrawCard() = 0;
    virtual void ShuffleCards() = 0;
    virtual std::vector<std::vector<CardType>> InitializePlayerCards() = 0;
    virtual void PrintCardPool() = 0;
};

class CardPool : public ICardPool {
   public:
    CardPool() {}
    ~CardPool() {}
    virtual void PutBackBomb(int pos) {
        if (pos == -1) {
            mCards.push_back(Bomb);
        } else {
            mCards.insert(mCards.begin(), pos - 1, Bomb);
        }
    }
    virtual CardType DrawCard() {
        auto card = mCards.front();
        mCards.erase(mCards.begin());
        return card;
    }
    virtual void ShuffleCards() {
        random_shuffle(mCards.begin(), mCards.end());
    }
    virtual std::vector<std::vector<CardType>> InitializePlayerCards() {
        std::map<CardType, int> CardMap{
            {Skip, SkipNum},
            {Shirk, ShirkNum},
            {Reverse, ReverseNum},
            {Predict, PredictNum},
            {SeeThrough, SeeThroughNum},
            {Swap, SwapNum},
            {GetBottom, GetBottomNum},
            {Shuffle, ShuffleNum},
            {Extort, ExtortNum},
            {BombDisposal, BombDisposal - BombNum - 1}};
        for (auto it : CardMap) {
            for (int i = 0; i < it.second; ++i) 
                mCards.emplace_back(it.first);
        }
        random_shuffle(mCards.begin(), mCards.end());
        std::vector<std::vector<CardType>> out;
        for (int i = 0; i < BombNum + 1; ++i) {
            out.emplace_back(mCards.begin(), mCards.begin() + InitialCardsNum - 1);
            out.back().push_back(BombDisposal);
            mCards.erase(mCards.begin(), mCards.begin() + InitialCardsNum - 1);
        }
        for (int i = 0; i < BombNum; ++i) {
            mCards.push_back(Bomb);
        }
        random_shuffle(mCards.begin(), mCards.end());
        return out;
    }
    virtual void PrintCardPool() {
        std::cout << "Game Card Pool\n";
        for (auto it : mCards) {
            std::cout << CardNames[it] << " ";
        }
        std::cout << std::endl;
    }

   private:
    std::vector<CardType> mCards;
};