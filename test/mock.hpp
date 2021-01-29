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
    MOCK_METHOD(int, PredictBombPos, (), (override));
    MOCK_METHOD(std::vector<CardType>, SeeThroughCards, (), (override));
    MOCK_METHOD(void, ShuffleCards, (), (override));
    MOCK_METHOD(std::vector<std::vector<CardType>>, InitializePlayerCards, (),
                (override));
    MOCK_METHOD(void, PrintCardPool, (), (override));
};

class MockProvider : public IProvider {
   public:
    MOCK_METHOD(void, SendStartGame,
                (int roomid, int uid, const std::vector<CardType>&,
                 const std::vector<int>&),
                (override));
    MOCK_METHOD(void, SendRoundStart,
                (int roomid, int uid, const std::vector<int>&), (override));
    MOCK_METHOD(void, SendCardOperationRespond,
                (int roomid, int uid, ExplodingKittensProto::CardType card,
                 const std::vector<int>& uids, int targetuid),
                (override));
    MOCK_METHOD(void, SendSwapResult,
                (int roomid, int uid, const std::vector<CardType>&),
                (override));
    MOCK_METHOD(void, SendExtortResult,
                (int roomid, int uid, CardType card, int srcid, int dstid),
                (override));
    MOCK_METHOD(void, SendPredictResult, (int roomid, int uid, int pos),
                (override));
    MOCK_METHOD(void, SendSeeThroughResult,
                (int roomid, int uid, const std::vector<CardType>&),
                (override));
    MOCK_METHOD(void, SendDrawResult, (int roomid, int uid, CardType card),
                (override));
    MOCK_METHOD(void, SendKO, (int roomid, int uid, std::vector<int>& uids),
                (override));
};

}  // namespace ExplodingKittens::Test
