#pragma once
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
};
struct PlayCardBombDisposal {
    CardType Card{BombDisposal};
};
struct SelectExtortCard {
    int SrcUid{0};
    int TargetUid{0};
};
struct GetExtortCard {};
struct MyTurn {};

struct DrawCard {
    CardType Card{Bomb};
};

struct AskExtortCard {
    int ExtortSrcUid{0};
    AskExtortCard(int srcid) : ExtortSrcUid(srcid) {}
};

struct ExtortCardSelected {
    CardType Card{Bomb};
};

struct Player_ : public msm::front::state_machine_def<Player_> {
    Player_(int uid, Game& game) : mUid(uid), mGame(game) {}

    // entry and exit of state: log
    template <class Event, class FSM>
    void on_entry(Event const&, FSM&) {
        std::cout << "entering: Player" << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM&) {
        std::cout << "leaving: Player" << std::endl;
    }

    // The list of FSM states
    struct Exploding : public msm::front::state<> {};

    struct Stopped : public msm::front::state<> {};

    struct Playing : public msm::front::state<> {};

    struct Extorted : public msm::front::state<> {
        int ExtortSrcId{0};
    };

    struct Died : public msm::front::state<> {};

    // the initial state of the player SM. Must be defined
    typedef Stopped initial_state;

// transition actions
// you can reuse them in another machine if you wish
#define action(name)                                                           \
    struct name {                                                              \
        template <class EVT, class FSM, class SourceState, class TargetState>  \
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&); \
    };
    action(EndOfTurn) action(Reverse) action(SeeThrough) action(Predict)
        action(Swap) action(Shuffle) action(HandleExploding) action(StoreCard)
            action(SelectExtortTarget) action(AskExtortCard)
                action(ExtortCardSelected)
#undef action

#define guard(name)                                                            \
    struct name {                                                              \
        template <class EVT, class FSM, class SourceState, class TargetState>  \
        bool operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&); \
    };
        // guard conditions
        guard(DoesCardExist) guard(IsShirkTarget) guard(IsBomb)
            guard(IsExtortTarget) guard(IsTargetAlive)
#undef guard
        // Transition table for player
        struct transition_table
        : mpl::vector<
              //    Start     Event         Next      Action Guard
              //  +---------+------------------+---------+---------------------------+----------------------+
              // stopped phase
              Row<Stopped, MyTurn, Playing, none, none>,
              Row<Stopped, GetExtortCard, Playing, none, none>,
              Row<Stopped, AskExtortCard, Extorted, AskExtortCard,
                  IsExtortTarget>,
              // playing card phase
              Row<Playing, PlayCardSkip, Stopped, EndOfTurn, DoesCardExist>,
              Row<Playing, PlayCardShirk, Stopped, EndOfTurn, DoesCardExist>,
              Row<Playing, PlayCardShirk, Playing, none,
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
              Row<Playing, PlayCardGetBottom, Stopped, none, DoesCardExist>,
              Row<Playing, PlayCardGetBottom, Exploding, HandleExploding,
                  DoesCardExist>,
              // draw card phase
              Row<Playing, DrawCard, Stopped, StoreCard, none>,
              Row<Playing, DrawCard, Exploding, none, IsBomb>,
              // extorted phase
              Row<Extorted, ExtortCardSelected, Stopped, ExtortCardSelected,
                  DoesCardExist>,
              // exploding phase
              Row<Exploding, PlayCardBombDisposal, Died, none, none>,
              Row<Exploding, PlayCardBombDisposal, Stopped, HandleExploding,
                  DoesCardExist>
              //  +---------+-------------+---------+---------------------------+----------------------+
              > {
    };
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
    Game& mGame;
};

bool IsPlayerDied(const Player& player) {
    return player.get_state_by_id(*player.current_state()) ==
           player.get_state<Player_::Died*>();
}

}  // namespace ExplodingKittens