#include "grpc_provider.hpp"

namespace ExplodingKittens {
using Core::ProviderMsg;
using ExplodingKittensProto::Skip;
using ExplodingKittensProto::UserOperation;

#define TransitionFor(MsgT) \
    template <>             \
    template <>             \
    void Provider::Transition<MsgT>(const MsgT& msg)

TransitionFor(ExplodingKittensProto::Skip);
TransitionFor(ExplodingKittensProto::Shirk);
TransitionFor(ExplodingKittensProto::Reverse);
TransitionFor(ExplodingKittensProto::Predict);
TransitionFor(ExplodingKittensProto::SeeThrough);
TransitionFor(ExplodingKittensProto::SwapCards);
TransitionFor(ExplodingKittensProto::GetBottom);
TransitionFor(ExplodingKittensProto::Shuffle);
TransitionFor(ExplodingKittensProto::Extort);
TransitionFor(ExplodingKittensProto::ExtortedRespond);
TransitionFor(ExplodingKittensProto::BombDisposal);
TransitionFor(ExplodingKittensProto::DrawCard);

void Provider::Start() {
    mStream = mStub->Provider(&mContext);
    Register();
    Core::ProviderMsg providerMsg;
    while (mStream->Read(&providerMsg)) {
        switch (providerMsg.Msg_case()) {
            case ProviderMsg::kRegisterRet: {
                const auto& registerRet = providerMsg.registerret();
                if (registerRet.err() != Core::ErrorNumber::OK) {
                    throw std::runtime_error("Register fail!");
                }
            }

            case ProviderMsg::kStartGameArgs: {
                const auto& startGameArgs = providerMsg.startgameargs();
                const auto& grpcUids = startGameArgs.userid();
                std::vector<int> uids{grpcUids.begin(), grpcUids.end()};
                auto roomid = startGameArgs.roomid();
                mGameMap.emplace(std::make_pair(
                    roomid, GameAndCardPool(roomid, uids, this)));
            }

            case ProviderMsg::kCloseGameArgs: {
                throw std::runtime_error("unimplemented CloseGameArgs!");
            }

            case ProviderMsg::kQueryStateArgs: {
                throw std::runtime_error("unimplemented QueryStateArgs!");
            }

            case ProviderMsg::kUserOperationArgs: {
                const auto& userOperationArgs = providerMsg.useroperationargs();
                ExplodingKittensProto::UserOperation userOperation;
                userOperationArgs.custom().UnpackTo(&userOperation);
                HandleUserMsg(userOperationArgs.roomid(),
                              userOperationArgs.userid(), userOperation);
            }
        }
    }
}

void Provider::SendStartGame(int roomid, int uid,
                             const std::vector<CardType>& cards,
                             const std::vector<int>& uids) {
    Core::ProviderMsg startGameMsg;
    auto* startGameArgs = startGameMsg.mutable_notifymsgargs();
    startGameArgs->set_roomid(roomid);
    startGameArgs->set_userid(uid);
    ExplodingKittensProto::NotifyMsg notifyStartGameMsg;

    ExplodingKittensProto::GameStart* gameStart =
        notifyStartGameMsg.mutable_gamestart();
    gameStart->set_userid(uid);
    *(gameStart->mutable_userids()) =
        google::protobuf::RepeatedField<google::protobuf::int32>(uids.begin(),
                                                                 uids.end());
    *(gameStart->mutable_cards()) =
        google::protobuf::RepeatedField<google::protobuf::int32>(cards.begin(),
                                                                 cards.end());

    startGameArgs->mutable_custom()->PackFrom(notifyStartGameMsg);

    mStream->Write(startGameMsg);
}

void Provider::SendRoundStart(int roomid, int uid,
                              const std::vector<int>& uids) {
    Core::ProviderMsg roundStartMsg;
    auto* roundStartArgs = roundStartMsg.mutable_notifymsgargs();
    roundStartArgs->set_roomid(roomid);
    ExplodingKittensProto::NotifyMsg notifyRoundStartMsg;

    ExplodingKittensProto::RoundStart* roundStart =
        notifyRoundStartMsg.mutable_roundstart();
    roundStart->set_userid(uid);

    for (auto id : uids) {
        roundStartArgs->set_userid(id);
        roundStartArgs->mutable_custom()->PackFrom(notifyRoundStartMsg);

        mStream->Write(roundStartMsg);
    }
}

void Provider::Register() {
    Core::ProviderMsg registerMsg;
    auto* registerArgs = registerMsg.mutable_registerargs();
    *(registerArgs->mutable_id()) = mId;
    *(registerArgs->mutable_gamename()) = mGameName;
    mStream->Write(registerMsg);
}

void Provider::HandleUserMsg(int roomid, int uid,
                             const ExplodingKittensProto::UserOperation& msg) {
    Game& game = mGameMap[roomid].mGame;
    for (auto& player : game.mPlayers) {
        if (player.mUid == uid) {
            switch (msg.Operation_case()) {
                case UserOperation::kSkip:
                    return Transition<ExplodingKittensProto::Skip>(msg.skip());
                case UserOperation::kShirk:
                    return Transition<ExplodingKittensProto::Shirk>(
                        msg.shirk());
                case UserOperation::kReverse:
                    return Transition<ExplodingKittensProto::Reverse>(
                        msg.reverse());
                case UserOperation::kPredict:
                    return Transition<ExplodingKittensProto::Predict>(
                        msg.predict());
                case UserOperation::kSeeThrough:
                    return Transition<ExplodingKittensProto::SeeThrough>(
                        msg.seethrough());
                case UserOperation::kSwapCards:
                    return Transition<ExplodingKittensProto::SwapCards>(
                        msg.swapcards());
                case UserOperation::kGetBottom:
                    return Transition<ExplodingKittensProto::GetBottom>(
                        msg.getbottom());
                case UserOperation::kShuffle:
                    return Transition<ExplodingKittensProto::Shuffle>(
                        msg.shuffle());
                case UserOperation::kExtort:
                    return Transition<ExplodingKittensProto::Extort>(
                        msg.extort());
                case UserOperation::kExtortedRespond:
                    return Transition<ExplodingKittensProto::ExtortedRespond>(
                        msg.extortedrespond());
                case UserOperation::kBombDisposal:
                    return Transition<ExplodingKittensProto::BombDisposal>(
                        msg.bombdisposal());
                case UserOperation::kDrawCard:
                    return Transition<ExplodingKittensProto::DrawCard>(
                        msg.drawcard());
            }
        }
    }
}

}  // namespace ExplodingKittens