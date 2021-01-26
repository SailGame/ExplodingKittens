#include "grpc_provider.hpp"

namespace ExplodingKittens{
using Core::ProviderMsg;
using ExplodingKittensProto::UserOperation;
void Provider::Start() {
    mStream = mStub->Provider(&mContext);
    Register();
    Core::ProviderMsg providerMsg;
    while(mStream->Read(&providerMsg)) {
        switch(providerMsg.Msg_case()) {
            case ProviderMsg::kRegisterRet:{
                const auto& registerRet = providerMsg.registerret();
                if(registerRet.err()!=Core::ErrorNumber::OK){
                    throw std::runtime_error("Register fail!");
                }
            }

            case ProviderMsg::kStartGameArgs:{
                const auto& startGameArgs = providerMsg.startgameargs();
                const auto& grpcUids = startGameArgs.userid();
                std::vector<int> uids{grpcUids.begin(),grpcUids.end()};
                auto roomid = startGameArgs.roomid();
                mGameMap.emplace(std::make_pair(roomid, GameAndCardPool(roomid, uids, this)));
            }

            case ProviderMsg::kCloseGameArgs:{
                throw std::runtime_error("unimplemented CloseGameArgs!");
            }

            case ProviderMsg::kQueryStateArgs:{
                throw std::runtime_error("unimplemented QueryStateArgs!");

            }

            case ProviderMsg::kUserOperationArgs:{
                const auto& userOperationArgs = providerMsg.useroperationargs();
                ExplodingKittensProto::UserOperation userOperation;
                userOperationArgs.custom().UnpackTo(&userOperation);
                HandleUserMsg(userOperationArgs.roomid(), userOperationArgs.userid(), userOperation);
            }


        }
        
    }
   

}

void Provider::SendStartGame(int roomid, int uid, const std::vector<CardType>& cards, const std::vector<int>& uids){
    Core::ProviderMsg startGameMsg;
    auto* startGameArgs = startGameMsg.mutable_notifymsgargs();
    startGameArgs->set_roomid(roomid);
    startGameArgs->set_userid(uid);
    ExplodingKittensProto::NotifyMsg notifyStartGameMsg;

    ExplodingKittensProto::GameStart* gameStart = notifyStartGameMsg.mutable_gamestart();
    gameStart->set_userid(uid);
    *(gameStart->mutable_userids()) = google::protobuf::RepeatedField<google::protobuf::int32>(uids.begin(),uids.end());
    *(gameStart->mutable_cards()) = google::protobuf::RepeatedField<google::protobuf::int32>(cards.begin(),cards.end());
    
    startGameArgs->mutable_custom()->PackFrom(notifyStartGameMsg);

    mStream->Write(startGameMsg);
}

void Provider::SendRoundStart(int roomid, int uid, const std::vector<int>& uids) {
    Core::ProviderMsg roundStartMsg;
    auto* roundStartArgs = roundStartMsg.mutable_notifymsgargs();
    roundStartArgs->set_roomid(roomid);
    ExplodingKittensProto::NotifyMsg notifyRoundStartMsg;

    ExplodingKittensProto::RoundStart* roundStart = notifyRoundStartMsg.mutable_roundstart();
    roundStart->set_userid(uid);

    for(auto id: uids){
        roundStartArgs->set_userid(id);
        roundStartArgs->mutable_custom()->PackFrom(notifyRoundStartMsg);

        mStream->Write(roundStartMsg);
    }
}

void Provider::Register(){
    Core::ProviderMsg registerMsg;
    auto* registerArgs = registerMsg.mutable_registerargs();
    *(registerArgs->mutable_id()) = mId;
    *(registerArgs->mutable_gamename()) = mGameName;
    mStream->Write(registerMsg);
}

void Provider::HandleUserMsg(int roomid,int uid,const ExplodingKittensProto::UserOperation& msg){
    Game& game  = mGameMap[roomid].mGame;
    for(auto& player: game.mPlayers){
        if(player.mUid == uid){
            switch (msg.Operation_case()) {
                case UserOperation::kSkip:
                case UserOperation::kShirk:
                case UserOperation::kReverse:
                case UserOperation::kPredict:
                case UserOperation::kSeeThrough:
                case UserOperation::kSwapCards:
                case UserOperation::kGetBottom:
                case UserOperation::kShuffle:
                case UserOperation::kExtort:
                case UserOperation::kExtortedRespond:
                case UserOperation::kBombDisposal:
                case UserOperation::kDrawCard:{

                }
            }
        }
    }
}

}