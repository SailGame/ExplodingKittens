#pragma once
#include <explodingkittens/explodingkittens.pb.h>

#include <algorithm>
#include <boost/mpl/vector.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/euml/common.hpp>
#include <boost/msm/front/euml/operator.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "card_pool.hpp"

namespace mpl = boost::mpl;
namespace msm = boost::msm;
using namespace msm::front;
using namespace msm::front::euml;

namespace ExplodingKittens {
class Game;
struct Player_;
typedef msm::back::state_machine<Player_> Player;

// events
struct PlayCardSkip {
    CardType Card{Skip};
};
struct PlayCardShirk {
    CardType Card{Shirk};
    int TargetUid{0};
    PlayCardShirk(int targetUid) : TargetUid(targetUid) {}
};
struct PlayCardReverse {
    CardType Card{Reverse};
};
struct PlayCardPredict {
    CardType Card{Predict};
};
struct PlayCardSeeThrough {
    CardType Card{SeeThrough};
};
struct PlayCardSwap {
    CardType Card{Swap};
    int TargetUid{0};
    PlayCardSwap(int targetUid) : TargetUid(targetUid) {}
};
struct PlayCardGetBottom {
    CardType Card{GetBottom};
};
struct PlayCardShuffle {
    CardType Card{Shuffle};
};
struct PlayCardExtort {
    CardType Card{Extort};
    int TargetUid{0};
    PlayCardExtort(int targetUid) : TargetUid(targetUid) {}
};
struct PlayCardBombDisposal {
    CardType Card{BombDisposal};
    int pos{1};
    PlayCardBombDisposal(int pos) : pos(pos) {}
};
struct SelectExtortCard {
    int SrcUid{0};
    int TargetUid{0};
};
struct GetExtortCard {};
struct EventMyTurn {};

struct DrawCard {};

struct AskExtortCard {
    int ExtortSrcUid{0};
    AskExtortCard(int srcid) : ExtortSrcUid(srcid) {}
};

struct ExtortCardSelected {
    CardType Card{Bomb};
    ExtortCardSelected(CardType card) : Card(card) {}
};
// flags
struct Exploded {};
struct Player_ : public msm::front::state_machine_def<Player_> {
    Player_(int uid, Game &game) : mUid(uid), mGame(game) {}

    // entry and exit of state: log
    template <class Event, class FSM>
    void on_entry(Event const &, FSM &) {
        std::cout << "entering: Player" << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const &, FSM &) {
        std::cout << "leaving: Player" << std::endl;
    }

    // The list of FSM states
    struct Exploding : public msm::front::state<> {};

    struct Stopped : public msm::front::state<> {};

    struct Playing : public msm::front::state<> {};

    struct Extorted : public msm::front::state<> {
        int ExtortSrcUId{0};
    };

    struct Died : public msm::front::state<> {
        typedef mpl::vector1<Exploded> flag_list;
    };

    // the initial state of the player SM. Must be defined
    typedef Stopped initial_state;

// transition actions
// you can reuse them in another machine if you wish
#define action                                                            \
    template <class EVT, class FSM, class SourceState, class TargetState> \
    void operator()(EVT const &evt, FSM &fsm, SourceState &sstate,        \
                    TargetState &tstate)

    struct RoundStart {
        action {
            fsm.mGame.mProvider->SendRoundStart(fsm.mGame.mRoomId, fsm.mUid);
        }
    };
    struct EndOfTurn {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::SKIP);
            auto SkipIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(SkipIt);
            fsm.mGame.NextPlayer();
        }
    };
    struct ShirkItself {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::SHIRK,
                fsm.mUid);
            auto ShirkIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(ShirkIt);
            // TODO: tell user shirked
        }
    };
    struct ShirkOther {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::SHIRK,
                evt.TargetUid);
            auto ShirkIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(ShirkIt);
            for (int i = 0; i < fsm.mGame.mPlayers.size(); ++i) {
                if (fsm.mGame.mPlayers[i].mUid == evt.TargetUid) {
                    fsm.mGame.mPlayingPlayerPos = i;
                    fsm.mGame.mPlayers[i].process_event(EventMyTurn());
                }
            }
        }
    };
    struct Reverse {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::REVERSE);
            auto ReverseIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(ReverseIt);
            fsm.mGame.mClockwise = !fsm.mGame.mClockwise;
            fsm.mGame.NextPlayer();
        }
    };
    struct SeeThrough {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid,
                ExplodingKittensProto::SEE_THROUGH);
            auto SeeThroughIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(SeeThroughIt);
            fsm.mGame.mProvider->SendSeeThroughResult(
                fsm.mGame.mRoomId, fsm.mUid,
                fsm.mGame.mCardPool->SeeThroughCards());
        }
    };
    struct Predict {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::PREDICT);
            auto PredictIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(PredictIt);
            fsm.mGame.mProvider->SendPredictResult(
                fsm.mGame.mRoomId, fsm.mUid,
                fsm.mGame.mCardPool->PredictBombPos());
        }
    };
    struct Swap {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::SWAPCARDS,
                evt.TargetUid);
            auto SwapIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(SwapIt);
            for (auto &player : fsm.mGame.mPlayers) {
                if (player.mUid == evt.TargetUid) {
                    swap(fsm.mCards, player.mCards);
                    fsm.mGame.mProvider->SendSwapResult(fsm.mGame.mRoomId,
                                                        fsm.mUid, fsm.mCards);
                    fsm.mGame.mProvider->SendSwapResult(
                        fsm.mGame.mRoomId, player.mUid, player.mCards);
                }
            }
        }
    };
    struct GetBottom {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::GET_BOTTOM);
            auto GetBottomIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(GetBottomIt);
            fsm.mCards.push_back(fsm.mGame.mCardPool->Back());
            fsm.mGame.mCardPool->PopBack();
            // TODO: tell use current cards
            fsm.mGame.NextPlayer();
        }
    };
    struct GetBottomBomb {
        action {
            auto GetBottomIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(GetBottomIt);
            fsm.mGame.mCardPool->PopBack();
            // TODO: tell use get bottom bomb
        }
    };
    struct Shuffle {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::SHUFFLE);
            auto ShuffleIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(ShuffleIt);
            fsm.mGame.mCardPool->ShuffleCards();
        }
    };
    struct HandleExploding {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid,
                ExplodingKittensProto::BOMB_DISPOSAL);
            auto BombDisposalIt =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(BombDisposalIt);
            fsm.mGame.mCardPool->PutBackBomb(evt.pos);
            // TODO: limit the position
            fsm.mGame.NextPlayer();
        }
    };
    struct StoreCard {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::NONE);
            fsm.mCards.push_back(fsm.mGame.mCardPool->Front());
            fsm.mGame.mCardPool->PopFront();
            fsm.mGame.mProvider->SendDrawResult(fsm.mGame.mRoomId, fsm.mUid,
                                                fsm.mCards.back());
            fsm.mGame.NextPlayer();
        }
    };
    struct SelectExtortTarget {
        action {
            // if received, broadcast to every user: extort target
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::EXTORT,
                evt.TargetUid);
            for (auto &player : fsm.mGame.mPlayers) {
                if (player.mUid == evt.TargetUid)
                    player.process_event(AskExtortCard(fsm.mUid));
            }
        }
    };
    struct AskExtortCard_A {
        action { tstate.ExtortSrcUId = evt.ExtortSrcUid; }
    };
    struct GetExtortCard_A {
        action {
            // src add extorted card
            fsm.mCards.push_back(evt.card);
        }
    };
    struct ExtortCardSelected {
        action {
            fsm.mGame.mProvider->SendCardOperationRespond(
                fsm.mGame.mRoomId, fsm.mUid, ExplodingKittensProto::EXTORT,
                fsm.mUid);  // targetuid = -1 means extorted, user gives a card
            // dst erase extorted card
            auto pos =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(pos);
            for (auto player : fsm.mGame.mPlayers) {
                if (player.mUid == sstate.ExtortSrcUid) {
                    fsm.mGame.mProvider->SendExtortResult(
                        fsm.mGame.mRoomId, sstate.ExtortSrcUid, evt.Card,
                        sstate.ExtortSrcUid, fsm.mUid);
                    player.process_event(GetExtortCard(evt.Card));
                }
            }
        }
    };
    struct GameOver {
        action { fsm.mGame.mProvider->SendKO(fsm.mGame.mRoomId, fsm.mUid); }
    };
#undef action

#define guard                                                             \
    template <class EVT, class FSM, class SourceState, class TargetState> \
    bool operator()(EVT const &evt, FSM &fsm, SourceState &sstate,        \
                    TargetState &tstate)

    // guard conditions
    struct DoesCardExist {
        guard {
            return std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card) !=
                   fsm.mCards.end();
        }
    };
    struct IsShirkTarget {
        guard { return evt.TargetUid == fsm.mUid; }
    };
    struct IsFrontBomb {
        guard { return fsm.mGame.mCardPool->Front() == Bomb; }
    };
    struct IsBackBomb {
        guard { return fsm.mGame.mCardPool->Back() == Bomb; }
    };
    struct IsExtortTarget {
        guard { return evt.ExtortSrcUid == fsm.mUid; }
    };
    struct IsTargetAlive {
        guard {
            for (auto player : fsm.mGame.mPlayers) {
                if (player.mUid == evt.TargetUid) return true;
            }
            return false;
        }
    };

#undef guard
    // Transition table for player
    struct transition_table
        : mpl::vector<
              //    Start     Event         Next      Action Guard
              //  +---------+------------------+---------+---------------------------+----------------------+
              // stopped phase
              Row<Stopped, EventMyTurn, Playing, RoundStart, none>,
              Row<Stopped, GetExtortCard, Playing, GetExtortCard_A, none>,
              Row<Stopped, AskExtortCard, Extorted, AskExtortCard_A,
                  IsExtortTarget>,
              // playing card phase
              Row<Playing, PlayCardSkip, Stopped, EndOfTurn, DoesCardExist>,
              Row<Playing, PlayCardShirk, Stopped, ShirkOther, DoesCardExist>,
              Row<Playing, PlayCardShirk, Playing, ShirkItself,
                  And_<IsShirkTarget, DoesCardExist>>,
              Row<Playing, PlayCardReverse, Stopped, Reverse, DoesCardExist>,
              Row<Playing, PlayCardSeeThrough, Playing, SeeThrough,
                  DoesCardExist>,
              Row<Playing, PlayCardPredict, Playing, Predict, DoesCardExist>,
              Row<Playing, PlayCardSwap, Playing, Swap,
                  And_<DoesCardExist, IsTargetAlive>>,
              Row<Playing, PlayCardShuffle, Playing, Shuffle, DoesCardExist>,
              // extort
              Row<Playing, PlayCardExtort, Stopped, SelectExtortTarget,
                  And_<DoesCardExist, IsTargetAlive>>,
              Row<Playing, PlayCardGetBottom, Stopped, GetBottom,
                  DoesCardExist>,
              Row<Playing, PlayCardGetBottom, Exploding, GetBottomBomb,
                  And_<DoesCardExist, IsBackBomb>>,
              // draw card phase
              Row<Playing, DrawCard, Stopped, StoreCard, none>,
              Row<Playing, DrawCard, Exploding, none, IsFrontBomb>,
              // extorted phase
              Row<Extorted, ExtortCardSelected, Stopped, ExtortCardSelected,
                  DoesCardExist>,
              // exploding phase
              Row<Exploding, PlayCardBombDisposal, Died, GameOver, none>,
              Row<Exploding, PlayCardBombDisposal, Stopped, HandleExploding,
                  DoesCardExist>
              //  +---------+-------------+---------+---------------------------+----------------------+
              > {};
    // Replaces the default no-transition response.
    template <class FSM, class Event>
    void no_transition(Event const &e, FSM &, int state) {
        std::cout << "no transition from state " << state << " on event "
                  << typeid(e).name() << std::endl;
    }
    int mUid{0};
    std::vector<CardType> mCards;
    void PrintCards() {
        std::cout << "Player " << mUid << " own " << mCards.size()
                  << " cards:\n";
        for (auto it : mCards) {
            std::cout << CardTypeToString(it) << " ";
        }
        std::cout << std::endl;
    }
    Game &mGame;
};  // namespace ExplodingKittens

}  // namespace ExplodingKittens