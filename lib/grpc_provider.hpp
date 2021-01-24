#pragma once

#include <core/core.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include "game.hpp"

namespace ExplodingKittens {

struct GameAndCardPool{
  explicit GameAndCardPool(const std::vector<int> &uids): CardPool(), Game(uids, mCardPool){}
  CardPool mCardPool;
  Game mGame;
};

class Provider : public IProvider{
public:
    Provider(std::shared_ptr<grpc::Channel> channel)
      : mStub(Core::GameCore::NewStub(channel)),mId("ExplodingKittens") {}
    void Start();
 private:
  void Register();
  void HandleUserMsg(int,int,const google::protobuf::Any&);
  std::unique_ptr<Core::GameCore::Stub> mStub;
   grpc::ClientContext mContext;
   std::unique_ptr<grpc::ClientReaderWriter<Core::ProviderMsg, Core::ProviderMsg>> mStream;
    std::string mId;
    std::string mGameName {"ExplodingKittens"};
    std::map<int, GameAndCardPool> mGameMap;
  };


}