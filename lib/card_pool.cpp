#include "card_pool.hpp"

namespace ExplodingKittens {
const std::map<CardType, int> CardMap{
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

void CardPool::PutBackBomb(int pos) {
    if (pos == -1) {
        mCards.push_back(Bomb);
    } else {
        mCards.insert(mCards.begin(), pos - 1, Bomb);
    }
}
CardType CardPool::DrawCard() {
    auto card = mCards.front();
    mCards.erase(mCards.begin());
    return card;
}
void CardPool::ShuffleCards() { random_shuffle(mCards.begin(), mCards.end()); }
std::vector<std::vector<CardType>> CardPool::InitializePlayerCards() {
    for (auto it : CardMap) {
        for (int i = 0; i < it.second; ++i) mCards.emplace_back(it.first);
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
void CardPool::PrintCardPool() {
    std::cout << "Game Card Pool\n";
    for (auto it : mCards) {
        std::cout << CardTypeToString(it) << " ";
    }
    std::cout << std::endl;
}

}  // namespace ExplodingKittens