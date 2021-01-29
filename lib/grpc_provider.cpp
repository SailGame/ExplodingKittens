#include "grpc_provider.hpp"

namespace ExplodingKittens {
using Core::ProviderMsg;
using ExplodingKittensProto::Skip;
using ExplodingKittensProto::UserOperation;

static CardType ProtoCardTypeToCardType(ExplodingKittensProto::CardType card) {
    return CardType(int(card));
}

static ExplodingKittensProto::CardType CardTypeToProtoCardType(CardType card) {
    return ExplodingKittensProto::CardType(int(card));
}

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
                    return Transition<ExplodingKittensProto::Skip>(game, player,
                                                                   msg.skip());
                case UserOperation::kShirk:
                    return Transition<ExplodingKittensProto::Shirk>(
                        game, player, msg.shirk());
                case UserOperation::kReverse:
                    return Transition<ExplodingKittensProto::Reverse>(
                        game, player, msg.reverse());
                case UserOperation::kPredict:
                    return Transition<ExplodingKittensProto::Predict>(
                        game, player, msg.predict());
                case UserOperation::kSeeThrough:
                    return Transition<ExplodingKittensProto::SeeThrough>(
                        game, player, msg.seethrough());
                case UserOperation::kSwapCards:
                    return Transition<ExplodingKittensProto::SwapCards>(
                        game, player, msg.swapcards());
                case UserOperation::kGetBottom:
                    return Transition<ExplodingKittensProto::GetBottom>(
                        game, player, msg.getbottom());
                case UserOperation::kShuffle:
                    return Transition<ExplodingKittensProto::Shuffle>(
                        game, player, msg.shuffle());
                case UserOperation::kExtort:
                    return Transition<ExplodingKittensProto::Extort>(
                        game, player, msg.extort());
                case UserOperation::kExtortedRespond:
                    return Transition<ExplodingKittensProto::ExtortedRespond>(
                        game, player, msg.extortedrespond());
                case UserOperation::kBombDisposal:
                    return Transition<ExplodingKittensProto::BombDisposal>(
                        game, player, msg.bombdisposal());
                case UserOperation::kDrawCard:
                    return Transition<ExplodingKittensProto::DrawCard>(
                        game, player, msg.drawcard());
            }
        }
    }
}

void Provider::SendNotifyMsg(
    int roomid, int uid, const ExplodingKittensProto::NotifyMsg& notifyMsg) {
    Core::ProviderMsg providerMsg;
    auto providerArgs = providerMsg.mutable_notifymsgargs();
    providerArgs->set_roomid(roomid);
    providerArgs->set_userid(uid);
    providerArgs->mutable_custom()->PackFrom(notifyMsg);

    mStream->Write(providerMsg);
}

void Provider::SendCardOperationRespond(int roomid, int uid,
                                        ExplodingKittensProto::CardType card,
                                        const std::vector<int>& uids,
                                        int targetuid) {
    ExplodingKittensProto::NotifyMsg notifyOperRespondMsg;
    ExplodingKittensProto::CardOperation* cardOperation =
        notifyOperRespondMsg.mutable_cardoperation();
    cardOperation->set_userid(uid);
    cardOperation->set_card(card);
    cardOperation->set_targetuid(targetuid);
    cardOperation->set_isdrawcard(card == ExplodingKittensProto::NONE ? true
                                                                      : false);
    for (auto id : uids) {
        SendNotifyMsg(roomid, id, notifyOperRespondMsg);
    }
}

void Provider::SendSwapResult(int roomid, int uid,
                              const std::vector<CardType>& cards) {
    ExplodingKittensProto::NotifyMsg notifySwapRetMsg;
    ExplodingKittensProto::SwapResult* swapResult =
        notifySwapRetMsg.mutable_swapresult();
    *(swapResult->mutable_cards()) =
        google::protobuf::RepeatedField<google::protobuf::int32>(cards.begin(),
                                                                 cards.end());
    SendNotifyMsg(roomid, uid, notifySwapRetMsg);
}

void Provider::SendExtortResult(int roomid, int uid, CardType card, int srcid,
                                int dstid) {
    ExplodingKittensProto::NotifyMsg notifyExtortRetMsg;
    ExplodingKittensProto::ExtortResult* extortResult =
        notifyExtortRetMsg.mutable_extortresult();
    extortResult->set_card(CardTypeToProtoCardType(card));
    extortResult->set_dstuid(dstid);
    extortResult->set_srcuid(srcid);

    SendNotifyMsg(roomid, uid, notifyExtortRetMsg);
}

void Provider::SendDrawResult(int roomid, int uid, CardType card) {
    ExplodingKittensProto::NotifyMsg notifyDrawRetMsg;

    ExplodingKittensProto::DrawResult* drawResult =
        notifyDrawRetMsg.mutable_drawresult();
    drawResult->set_card(CardTypeToProtoCardType(card));

    SendNotifyMsg(roomid, uid, notifyDrawRetMsg);
}

void Provider::SendSeeThroughResult(int roomid, int uid,
                                    const std::vector<CardType>& cards) {
    ExplodingKittensProto::NotifyMsg notifySeeThroughMsg;

    ExplodingKittensProto::SeeThroughResult* seeThroughResult =
        notifySeeThroughMsg.mutable_seethroughresult();
    *(seeThroughResult->mutable_cards()) =
        google::protobuf::RepeatedField<google::protobuf::int32>(cards.begin(),
                                                                 cards.end());
    SendNotifyMsg(roomid, uid, notifySeeThroughMsg);
}

void Provider::SendKO(int roomid, int uid, std::vector<int>& uids) {
    ExplodingKittensProto::NotifyMsg notifykoMsg;

    ExplodingKittensProto::KO* ko = notifykoMsg.mutable_ko();
    ko->set_userid(uid);
    for (auto id : uids) {
        SendNotifyMsg(roomid, id, notifykoMsg);
    }
}

#define TransitionFor(MsgT)                                     \
    void Provider::Transition##MsgT(Game& game, Player& player, \
                                    const ExplodingKittensProto::MsgT& msg)

TransitionFor(Skip) { player.process_event(PlayCardSkip()); }

TransitionFor(Shirk) { player.process_event(PlayCardShirk(msg.targetuid())); }

TransitionFor(Reverse) { player.process_event(PlayCardReverse()); }

TransitionFor(Predict) { player.process_event(PlayCardPredict()); }

TransitionFor(SeeThrough) { player.process_event(PlayCardSeeThrough()); }

TransitionFor(SwapCards) {
    player.process_event(PlayCardSwap(msg.targetuid()));
}

TransitionFor(GetBottom) { player.process_event(PlayCardGetBottom()); }

TransitionFor(Shuffle) { player.process_event(PlayCardShuffle()); }

TransitionFor(Extort) { player.process_event(PlayCardExtort(msg.targetuid())); }

TransitionFor(ExtortedRespond) {
    auto card = ProtoCardTypeToCardType(msg.card());
    player.process_event(ExtortCardSelected(card));
}

TransitionFor(BombDisposal) {
    player.process_event(PlayCardBombDisposal(msg.pos()));
}

TransitionFor(DrawCard) { player.process_event(DrawCard()); }
#undef TransitionFor

}  // namespace ExplodingKittens