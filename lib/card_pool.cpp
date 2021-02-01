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
        mCards.insert(mCards.begin(), (pos - 1) % mCards.size(), Bomb);
    }
}
CardType CardPool::Front() { return mCards.front(); }
CardType CardPool::Back() { return mCards.back(); }
void CardPool::PopFront() { mCards.erase(mCards.begin()); }
void CardPool::PopBack() { mCards.erase(mCards.end() - 1); }
int CardPool::PredictBombPos() {
    auto bombIt = std::find(mCards.begin(), mCards.end(), Bomb);
    if (bombIt != mCards.end()) {
        return bombIt - mCards.begin() + 1;
    }
    throw std::runtime_error("Bomb don't exist!");
};
std::vector<CardType> CardPool::SeeThroughCards() {
    if (mCards.size() >= PredictCardNum)
        return std::vector<CardType>(mCards.begin(), mCards.begin() + 3);
    else
        return mCards;
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