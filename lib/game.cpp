#include "game.hpp"
namespace ExplodingKittens {
static bool IsPlayerDied(const Player &player) {
    return player.get_state_by_id(*player.current_state()) ==
           player.get_state<Player_::Died *>();
}

Game::Game(const std::vector<int> &uids, ICardPool &cardPool)
    : mCardPool(cardPool) {
    for (auto it : uids) mPlayers.emplace_back(it, *this);
}

void Game::GameStart() {
    auto cardsVecs = mCardPool.InitializePlayerCards();
    for (int i = 0; i < cardsVecs.size(); ++i) {
        mPlayers[i].mCards = cardsVecs[i];
        mPlayers[i].start();
    }
    mPlayingPlayerPos = 0;
    mPlayers[mPlayingPlayerPos].process_event(EventMyTurn());
    // TODO: send msg to each player: cards info, user info, game start
}

void Game::NextPlayer() {
    auto playerNum = mPlayers.size();
    if (mClockwise) {
        for (int i = 1; i < playerNum; ++i) {
            auto &player = mPlayers[(i + mPlayingPlayerPos) % playerNum];
            if (!IsPlayerDied(player)) {
                player.process_event(EventMyTurn());
                return;
            }
        }
    } else {
        for (int i = playerNum - 1; i > 0; --i) {
            auto &player = mPlayers[(i + mPlayingPlayerPos) % playerNum];
            if (!IsPlayerDied(player)) {
                player.process_event(EventMyTurn());
                return;
            }
        }
    }
    // TODO: here only one player left, send msg to all users to tell game over
}
}  // namespace ExplodingKittens
