#pragma once

#include <core/core.grpc.pb.h>
#include <explodingkittens/explodingkittens.pb.h>
#include <grpcpp/grpcpp.h>

#include "game.hpp"
#include "interface.hpp"

namespace ExplodingKittens {

struct GameAndCardPool {
    GameAndCardPool() {}
    explicit GameAndCardPool(int roomid, const std::vector<int>& uids,
                             IProvider* provider)
        : mCardPool(new CardPool()), mGame(roomid, uids, provider, mCardPool) {}
    CardPool* mCardPool{nullptr};
    Game mGame;
};

class Provider : public IProvider {
   public:
    Provider(std::shared_ptr<grpc::Channel> channel)
        : mStub(Core::GameCore::NewStub(channel)), mId("ExplodingKittens") {}
    void Start();
    virtual void SendStartGame(int roomid, int uid,
                               const std::vector<CardType>&,
                               const std::vector<int>&) override;
    virtual void SendRoundStart(int roomid, int uid) override;
    virtual void SendCardOperationRespond(int roomid, int uid,
                                          ExplodingKittensProto::CardType card,
                                          int targetuid = -1) override;
    virtual void SendSwapResult(int roomid, int uid,
                                const std::vector<CardType>&) override;
    virtual void SendExtortResult(int roomid, int uid, CardType card, int srcid,
                                  int dstid) override;
    virtual void SendDrawResult(int roomid, int uid, CardType card) override;
    virtual void SendPredictResult(int roomid, int uid, int pos) override;
    virtual void SendSeeThroughResult(int roomid, int uid,
                                      const std::vector<CardType>&) override;
    virtual void SendGetBomb(int roomid, int uid) override;
    virtual void SendKO(int roomid, int uid) override;

   private:
    void Register();
    void HandleUserMsg(int roomid, int uid,
                       const ExplodingKittensProto::UserOperation&);
    void SendNotifyMsg(int roomid, int uid,
                       const ExplodingKittensProto::NotifyMsg& notifyMsg);
    template <typename MsgT>
    void Transition(Game&, Player&, const MsgT& event) {
        throw std::runtime_error("Unsupported msg type");
    }
#define TransitionFor(MsgT)                           \
    void Transition##MsgT(Game& game, Player& player, \
                          const ExplodingKittensProto::MsgT& msg)
    TransitionFor(Skip);
    TransitionFor(Shirk);
    TransitionFor(Reverse);
    TransitionFor(Predict);
    TransitionFor(SeeThrough);
    TransitionFor(SwapCards);
    TransitionFor(GetBottom);
    TransitionFor(Shuffle);
    TransitionFor(Extort);
    TransitionFor(ExtortedRespond);
    TransitionFor(BombDisposal);
    TransitionFor(DrawCard);
#undef TransitionFor
    std::unique_ptr<Core::GameCore::Stub> mStub;
    grpc::ClientContext mContext;
    std::unique_ptr<
        grpc::ClientReaderWriter<Core::ProviderMsg, Core::ProviderMsg>>
        mStream;
    std::string mId;
    std::string mGameName{"ExplodingKittens"};
    std::map<int, GameAndCardPool> mGameMap;
};

}  // namespace ExplodingKittens