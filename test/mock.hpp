#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "interface.hpp"
namespace ExplodingKittens::Test {
class MockCardPool : public ICardPool {
   public:
    MOCK_METHOD(void, PutBackBomb, (int pos), (override));
    MOCK_METHOD(CardType, Front, (), (override));
    MOCK_METHOD(CardType, Back, (), (override));
    MOCK_METHOD(void, PopFront, (), (override));
    MOCK_METHOD(void, PopBack, (), (override));
    MOCK_METHOD(void, ShuffleCards, (), (override));
    MOCK_METHOD(std::vector<std::vector<CardType>>, InitializePlayerCards, (),
                (override));
    MOCK_METHOD(void, PrintCardPool, (), (override));
};

class MockProvider : public IProvider {
   public:
    MOCK_METHOD(void, SendStartGame, (int roomid, int uid, const std::vector<CardType>&, const std::vector<int>&), (override));
    MOCK_METHOD(void, SendRoundStart, (int roomid, int uid, const std::vector<int>&), (override));
};

}  // namespace ExplodingKittens::Test
