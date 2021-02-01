#pragma once
#include <stdexcept>
#include <string>
namespace ExplodingKittens {
enum CardType {
    None,
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

std::string CardTypeToString(CardType card);
}  // namespace ExplodingKittens