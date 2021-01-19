 #pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "state_machine.hpp"
#include "mock.hpp"
namespace mpl = boost::mpl;
namespace msm = boost::msm;
using namespace msm::front;
using namespace msm::front::euml;

namespace Test {
class ContainerFixture : public ::testing::Test {
   public:
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
    EXPECT_CALL(mMockCardPool, InitializePlayerCards());
    mGame.start();
    mGame.process_event(GameStart({1,2,3,4,5},mMockCardPool));
    for (auto it : mGame.mPlayers) {
        it.PrintCards();
    }
}

}  // namespace Test
