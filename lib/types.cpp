#include "types.hpp"
namespace ExplodingKittens {
std::string CardTypeToString(CardType card) {
    switch (card) {
        case Bomb:
            return "Bomb";
        case Shirk:
            return "Shirk";

        case Skip:
            return "Skip";

        case Reverse:
            return "Reverse";

        case Predict:
            return "Predict";

        case SeeThrough:
            return "SeeThrough";
        case Swap:
            return "Swap";

        case GetBottom:
            return "GetBottom";
        case Shuffle:
            return "Shuffle";

        case Extort:
            return "Extort";

        case BombDisposal:
            return "BombDisposal";
    }
    throw std::runtime_error("card type not exist!");
}
}  // namespace ExplodingKittens