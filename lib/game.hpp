#pragma once
#include "interface.hpp"
#include "player.hpp"
namespace ExplodingKittens {

class Game {
   public:
    Game(const std::vector<int> &uids, IProvider& provider, ICardPool &cardPool);
    void GameStart();
    void NextPlayer();
    std::vector<Player> mPlayers;
    bool mClockwise{true};
    int mPlayingPlayerPos{0};
    IProvider& mProvider;
    ICardPool &mCardPool;
};
}  // namespace ExplodingKittens