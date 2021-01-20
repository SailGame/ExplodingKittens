#pragma once
#include "player.hpp"
#include "interface.hpp"
namespace ExplodingKittens {

class Game {
   public:
    Game(const std::vector<int>& uids, ICardPool& cardPool);
    void GameStart();
    void NextPlayer();
    std::vector<Player> mPlayers;
    bool mClockwise{true};
    int mPlayingPlayerPos{0};
    ICardPool& mCardPool;
};
}  // namespace ExplodingKittens