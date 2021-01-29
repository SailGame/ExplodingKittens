#pragma once
#include <explodingkittens/explodingkittens.pb.h>

#include <vector>

#include "types.hpp"
namespace ExplodingKittens {

class IProvider {
   public:
    virtual void SendStartGame(int roomid, int uid,
                               const std::vector<CardType>&,
                               const std::vector<int>&) = 0;
    virtual void SendRoundStart(int roomid, int uid,
                                const std::vector<int>&) = 0;
    virtual void SendCardOperationRespond(int roomid, int uid,
                                          ExplodingKittensProto::CardType card,
                                          const std::vector<int>& uids,
                                          int targetuid = -1) = 0;
    virtual void SendSwapResult(int roomid, int uid,
                                const std::vector<CardType>&) = 0;
    virtual void SendExtortResult(int roomid, int uid, CardType card, int srcid,
                                  int dstid) = 0;
    virtual void SendSeeThroughResult(int roomid, int uid,
                                      const std::vector<CardType>&) = 0;
    virtual void SendDrawResult(int roomid, int uid, CardType card) = 0;
    virtual void SendKO(int roomid, int uid, std::vector<int>& uids) = 0;
};

class ICardPool {
   public:
    virtual void PutBackBomb(int pos) = 0;
    virtual CardType Front() = 0;
    virtual CardType Back() = 0;
    virtual void PopFront() = 0;
    virtual void PopBack() = 0;
    virtual std::vector<CardType> SeeThroughCards() = 0;
    virtual void ShuffleCards() = 0;
    virtual std::vector<std::vector<CardType>> InitializePlayerCards() = 0;
    virtual void PrintCardPool() = 0;
};
}  // namespace ExplodingKittens