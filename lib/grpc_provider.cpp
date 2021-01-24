#include "grpc_provider.hpp"

namespace ExplodingKittens{
void Provider::Start() {
    mStream = mStub->Provider(&mContext);
    Register();
    Core::ProviderMsg providerMsg;
    while(mStream->Read(&providerMsg)) {
        switch(providerMsg.Msg_case()) {
            case Core::ProviderMsg::kRegisterRet:{
                const auto& registerRet = providerMsg.registerret();
                if(registerRet.err()!=Core::ErrorNumber::OK){
                    throw std::runtime_error("Register fail!");
                }
            }

            case Core::ProviderMsg::kStartGameArgs:{
                const auto& startGameArgs = providerMsg.startgameargs();
                const auto& grpcUids = startGameArgs.userid();
                std::vector<int> uids{grpcUids.begin(),grpcUids.end()};
                mGameMap.emplace(std::make_pair(startGameArgs.roomid(), GameAndCardPool(*this, uids)));
            }

            case Core::ProviderMsg::kCloseGameArgs:{
                throw std::runtime_error("unimplemented CloseGameArgs!");
            }

            case Core::ProviderMsg::kQueryStateArgs:{
                throw std::runtime_error("unimplemented QueryStateArgs!");

            }

            case Core::ProviderMsg::kUserOperationArgs:{
                const auto& userOperationArgs = providerMsg.useroperationargs();
                HandleUserMsg(userOperationArgs.roomid(), userOperationArgs.userid(), userOperationArgs.custom());
            }


        }
        
    }
   

}

void Provider::Register(){
    Core::ProviderMsg registerMsg;
    auto* registerArgs = registerMsg.mutable_registerargs();
    *(registerArgs->mutable_id()) = mId;
    *(registerArgs->mutable_gamename()) = mGameName;
    mStream->Write(registerMsg);
}
}