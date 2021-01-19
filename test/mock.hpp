#pragma once
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include "CardPool.hpp"



class MockCardPool : public CardPool {
 public:
  MOCK_METHOD(void, PutBackBomb, (int pos), (override));
  MOCK_METHOD(CardType, DrawCard, (),(override));
  MOCK_METHOD(void, ShuffleCards, (), (override));
  MOCK_METHOD(std::vector<std::vector<CardType>>,InitializePlayerCards,(),(override));
};

