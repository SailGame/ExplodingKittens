#include "game.hpp"
#include "mock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

namespace mpl = boost::mpl;
namespace msm = boost::msm;
using namespace msm::front;
using namespace msm::front::euml;

namespace ExplodingKittens::Test {
const std::vector<int> Uids{1, 2, 3, 4, 5};
const std::vector<std::vector<CardType>> InitializedPlayerCards{
    {BombDisposal, Skip, Swap, Reverse, Shuffle},
    {BombDisposal, Reverse, GetBottom, Shirk, Extort},
    {BombDisposal, Shirk, Shuffle, Predict, Skip},
    {BombDisposal, Predict, Extort, SeeThrough, Reverse},
    {BombDisposal, SeeThrough, Skip, GetBottom, BombDisposal}};
class ContainerFixture : public ::testing::Test {
   public:
    ContainerFixture() : mGame(Uids, mMockCardPool) {}

    MockCardPool mMockCardPool;
    Game mGame;
    void SetUp() {
        // code here will execute just before the test ensues
    }

    void TearDown() {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }
};

TEST_F(ContainerFixture, CreateGame) {
    // test gamestart
    EXPECT_CALL(mMockCardPool, InitializePlayerCards())
        .WillOnce(testing::Return(InitializedPlayerCards));
    mGame.GameStart();
    // GRPC MOCK
    for (auto it : mGame.mPlayers) {
        it.PrintCards();
    }
    EXPECT_EQ(mGame.mPlayingPlayerPos, 0);
    auto currentPlayer = mGame.mPlayers.begin();
    
    // test skip
    currentPlayer->process_event(PlayCardSkip());
    EXPECT_EQ(currentPlayer->mCards.size(),4);
    EXPECT_EQ(mGame.mPlayingPlayerPos, 1);
    currentPlayer = currentPlayer++;
    //GRPC MOCk


}

}  // namespace ExplodingKittens::Test
