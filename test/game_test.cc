// Copyright [2020] <DeeEll-X/Veiasai>"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sys/mount.h>

#include <state_machine.hpp>
#include <string>

namespace Test {
class ContainerFixture : public ::testing::Test {
   public:
    ContainerFixture() : mGame({1, 2, 3, 4, 5, 6}) {}
    Game mGame;
    void SetUp() {
        // code here will execute just before the test ensues
    }

    void TearDown() {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }
};

TEST_F(ContainerFixture, Create) {}

}  // namespace Test
