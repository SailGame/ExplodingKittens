#include "player.hpp"

namespace ExplodingKittens {
template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::EndOfTurn::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                    TargetState&) {
    auto pos = std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
    fsm.mCards.erase(pos);
    fsm.mGame.NextPlayer();
    // TODO: tell user end of the turn
}

template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::Reverse::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                  TargetState&) {
    auto pos = std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
    fsm.mCards.erase(pos);
    fsm.mGame.mClockwise = !fsm.mGame.mClockwise;
    fsm.mGame.NextPlayer();
    // TODO: tell user end of the turn
}

template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::SeeThrough::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                     TargetState&) {
    auto pos = std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
    fsm.mCards.erase(pos);
    // TODO: tell user the first three cards
}

template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::Predict::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                  TargetState&) {
    auto pos = std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
    fsm.mCards.erase(pos);
    // TODO: tell user the pos of the first bomb
}

template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::Swap::operator()(EVT const& evt, FSM& fsm, SourceState&,
                               TargetState&) {
    auto pos = std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
    fsm.mCards.erase(pos);
    for (auto player : fsm.mGame.mPlayers) {
        if (player.mUid == evt.TargetUid) swap(fsm.mCards, player.mCards);
        // TODO: tell two user current cards
    }
}

template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::Shuffle::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                  TargetState&) {
    auto pos = std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
    fsm.mCards.erase(pos);
    fsm.mGame.mCardPool.ShuffleCards();
}

template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::HandleExploding::operator()(EVT const& evt, FSM& fsm,
                                          SourceState&, TargetState&) {
    auto pos = std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
    fsm.mCards.erase(pos);
    // TODO: ask user where to put bomb
    // handle user respond;
}

template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::StoreCard::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                    TargetState&) {
    fsm.mCards.push_back(evt.Card);
}

template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::SelectExtortTarget::operator()(EVT const& evt, FSM& fsm,
                                             SourceState&, TargetState&) {
    for (auto player : fsm.mGame.mPlayers) {
        if (player.mUid == evt.TargetUid)
            player.process_on(AskExtortCard(fsm.mUid));
    }
}

template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::AskExtortCard::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                        TargetState& tstate) {
    tstate.ExtortSrcId = evt.ExtortSrcUid;
    // TODO: ask user to give an card
}

template <class EVT, class FSM, class SourceState, class TargetState>
void Player_::ExtortCardSelected::operator()(EVT const& evt, FSM& fsm,
                                             SourceState& sstate,
                                             TargetState& tstate) {
    for (auto player : fsm.mGame.mPlayers) {
        if (player.mUid == sstate.ExtortSrcUid) {
            player.process_event(GetExtortCard(evt.Card));
        }
    }
}

// guard conditions
template <class EVT, class FSM, class SourceState, class TargetState>
bool Player_::DoesCardExist::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                        TargetState&) {
    return std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Cards) !=
           fsm.mCards.end();
}

template <class EVT, class FSM, class SourceState, class TargetState>
bool Player_::IsShirkTarget::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                        TargetState&) {
    return evt.TatgetUid == fsm.mUid;
}

template <class EVT, class FSM, class SourceState, class TargetState>
bool Player_::IsBomb::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                 TargetState&) {
    return evt.Card == Bomb;
}

template <class EVT, class FSM, class SourceState, class TargetState>
bool Player_::IsExtortTarget::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                         TargetState&) {
    return evt.TatgetUid == fsm.mUid;
}

template <class EVT, class FSM, class SourceState, class TargetState>
bool Player_::IsTargetAlive::operator()(EVT const& evt, FSM& fsm, SourceState&,
                                        TargetState&) {
    for (auto player : fsm.mGame.mPlayers) {
        if (player.mUid == evt.TargetUid) return true;
    }
    return false;
}

}  // namespace ExplodingKittens