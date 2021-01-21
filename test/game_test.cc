#include "game.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "mock.hpp"

namespace mpl = boost::mpl;
namespace msm = boost::msm;
using namespace msm::front;
using namespace msm::front::euml;

namespace ExplodingKittens::Test {
const std::vector<int> Uids{1, 2, 3, 4, 5};
const std::vector<std::vector<CardType>> InitializedPlayerCards{
    {BombDisposal, Skip, Swap, Reverse, Shuffle},
    {BombDisposal, GetBottom, Shirk, Reverse, Extort},
    {BombDisposal, GetBottom, Predict, Shirk, Shuffle},
    {BombDisposal, Shuffle, Shirk, Reverse, Swap},
    {BombDisposal, Reverse, Shirk, SeeThrough, BombDisposal}};
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

    // Player1: test skip
    currentPlayer->process_event(PlayCardSkip());
    EXPECT_EQ(currentPlayer->mCards.size(), 4);
    EXPECT_EQ(mGame.mPlayingPlayerPos, 1);
    EXPECT_FALSE(currentPlayer->is_flag_active<Exploded>());
    currentPlayer = std::next(currentPlayer);
    // GRPC MOCk

    // Player2: test getbottom - not Bomb
    EXPECT_CALL(mMockCardPool, Back())
        .WillRepeatedly(testing::Return(GetBottom));
    EXPECT_CALL(mMockCardPool, PopBack()).Times(1);
    currentPlayer->process_event(PlayCardGetBottom());
    EXPECT_EQ(currentPlayer->mCards.size(), 5);
    currentPlayer = std::next(currentPlayer);
    // GPRC MOCK

    // Player3: test getbottom - Bomb
    EXPECT_CALL(mMockCardPool, Back()).WillRepeatedly(testing::Return(Bomb));
    EXPECT_CALL(mMockCardPool, PopBack()).Times(1);
    currentPlayer->process_event(PlayCardGetBottom());
    EXPECT_EQ(currentPlayer->mCards.size(), 4);
    // GRPCMOCK
    EXPECT_CALL(mMockCardPool, PutBackBomb(-1)).Times(1);
    currentPlayer->process_event(PlayCardBombDisposal(-1));
    EXPECT_EQ(currentPlayer->mCards.size(), 3);
    currentPlayer = std::next(currentPlayer);
    // GRPCMOCK

    // Player4: test shuffle, drawcard
    EXPECT_CALL(mMockCardPool, ShuffleCards()).Times(1);
    currentPlayer->process_event(PlayCardShuffle());
    EXPECT_EQ(currentPlayer->mCards.size(), 4);
    EXPECT_CALL(mMockCardPool, Front())
        .WillRepeatedly(testing::Return(Shuffle));
    EXPECT_CALL(mMockCardPool, PopFront()).Times(1);
    currentPlayer->process_event(DrawCard());
    EXPECT_EQ(currentPlayer->mCards.size(), 5);
    currentPlayer = std::next(currentPlayer);
    // GRPCMOCK

    // Player 5: test Reverse
    currentPlayer->process_event(PlayCardReverse());
    EXPECT_FALSE(mGame.mClockwise);
    EXPECT_EQ(mGame.mPlayingPlayerPos, 3);
    currentPlayer = std::next(currentPlayer, -1);
    // GRPCMOCK

    // Player 4: test Swap,
    // auto SwapTargetIndex = 4;
    // auto SwapTargetPlayerCards = mGame.mPlayers[SwapTargetIndex].mCards;
    // mGame.mPlayers[SwapTargetIndex].PrintCards();
    // currentPlayer->process_event(PlayCardSwap(mGame.mPlayers[SwapTargetIndex].mUid));
    // EXPECT_EQ(currentPlayer->mCards, SwapTargetPlayerCards);

    // Player 4: test Shirk - Shirk itself, test Reverse
    currentPlayer->process_event(PlayCardShirk(4));
    EXPECT_EQ(mGame.mPlayingPlayerPos, 3);
    currentPlayer->process_event(PlayCardReverse());
    EXPECT_TRUE(mGame.mClockwise);
    EXPECT_EQ(mGame.mPlayingPlayerPos, 4);
    currentPlayer = std::next(currentPlayer);
    currentPlayer->PrintCards();

    // Player 5: test Shirk - Shirk player 1
    auto ShirkTargetIndex = 0;
    currentPlayer->process_event(
        PlayCardShirk(mGame.mPlayers[ShirkTargetIndex].mUid));
    EXPECT_EQ(mGame.mPlayingPlayerPos, 0);
    currentPlayer = mGame.mPlayers.begin();
}

}  // namespace ExplodingKittens::Test
