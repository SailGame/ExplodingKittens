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
const  std::vector<int> mUids{1, 2, 3, 4, 5};
class ContainerFixture : public ::testing::Test {
   public:
    ContainerFixture() : mGame(mUids, mMockCardPool) {}
    
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
    // mGame.process_event(GameStart({1,2,3,4,5},mMockCardPool));
    // for (auto it : mGame.mPlayers) {
    //     it.PrintCards();
    // }
}

}  // namespace ExplodingKittens::Test
