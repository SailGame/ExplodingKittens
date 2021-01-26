#pragma once
#include "interface.hpp"
#include "player.hpp"
namespace ExplodingKittens {

class Game {
   public:
    Game() {}
    Game(int roomid, const std::vector<int> &uids, IProvider* provider, ICardPool* cardPool);
    void GameStart();
    void NextPlayer();
    int mRoomId{0};
    std::vector<int> mUids{};
    std::vector<Player> mPlayers;
    bool mClockwise{true};
    int mPlayingPlayerPos{0};
    IProvider* mProvider{nullptr};
    ICardPool* mCardPool{nullptr};
};

}  // namespace ExplodingKittens