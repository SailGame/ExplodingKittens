#include <boost/mpl/vector.hpp>
#include <iostream>
#include <list>
// back-end
#include <boost/msm/back/state_machine.hpp>
// front-end
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/euml/common.hpp>
#include <boost/msm/front/functor_row.hpp>
// for And_ operator
#include <boost/msm/front/euml/operator.hpp>

namespace mpl = boost::mpl;
namespace msm = boost::msm;
using namespace msm::front;
using namespace msm::front::euml;
struct Player;
struct NextTurn {};
struct Game : public msm::front::state_machine_def<Player> {
    Game(std::vector<int> uids) {
        for (auto uid : uids) {
            Players.emplace_back(uid);
        }
    }

    // entry and exit of state: log
    template <class Event, class FSM>
    void on_entry(Event const &, FSM &) {
        std::cout << "entering: Player" << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const &, FSM &) {
        std::cout << "leaving: Player" << std::endl;
    }

    struct Playing : public msm::front::state<> {};

    struct Ended : public msm::front::state<> {};

    // the initial state of the player SM. Must be defined
    typedef Playing initial_state;

    struct OnlyOnePlayerLeft {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const &evt, FSM &fsm, SourceState &src,
                        TargetState &tgt) {
            return fsm.Players.size() == 1;
        }
    };
    // Transition table for player
    struct transition_table
        : mpl::vector<
              //    Start     Event         Next      Action Guard
              //  +---------+------------------+---------+---------------------------+----------------------+
              // stopped phase
              Row<Playing, NextTurn, Playing, none, none>,
              Row<Playing, NextTurn, Ended, none, OnlyOnePlayerLeft>
              //  +---------+-------------+---------+---------------------------+----------------------+
              > {};

    std::list<Player> Players;
    std::list<Player> DiedPlayers;
};

enum CardType {
    Bomb = -1,
    Skip,
    Shirk,
    Reverse,
    Predict,
    SeeThrough,
    Swap,
    GetBottom,
    Shuffle,
    Extort,
    BombDisposal
};
// events
struct PlayCardSkip {};
struct PlayCardShirk {
    int TargetUid{0};
};
struct PlayCardReverse {};
struct PlayCardPredict {};
struct PlayCardSeeThrough {};
struct PlayCardSwap {};
struct PlayCardGetBottom {};
struct PlayCardShuffle {};
struct PlayCardExtort {};
struct PlayCardBombDisposal {};
struct SelectExtortCard {
    int SrcUid{0};
    int TargetUid{0};
};
struct GetExtortCard {};
struct MyTurn {};

struct DrawCard {
    CardType Card{Bomb};
};
struct TimeOut {};

struct Player : public msm::front::state_machine_def<Player> {
    Player(int uid) : Uid(uid) {}

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

    struct Extorted : public msm::front::state<> {};

    struct Died : public msm::front::state<> {};

    // the initial state of the player SM. Must be defined
    typedef Stopped initial_state;

    // transition actions
    // you can reuse them in another machine if you wish

    // guard conditions
    struct IsExtortSrc {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const &evt, FSM &fsm, SourceState &src,
                        TargetState &tgt) {
            return evt.TargetUid == fsm.Uid;
        }
    };
    struct IsShirkTarget {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const &evt, FSM & fsm, SourceState &, TargetState &) {
            return evt.TatgetUid == fsm.Uid;
        }
    };
    struct IsBomb {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const &evt, FSM & fsm, SourceState &, TargetState &) {
            return evt.Card == Bomb;
        }
    };
    struct IsExtortTarget {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const &evt, FSM & fsm, SourceState &, TargetState &) {
            return evt.TatgetUid == fsm.Uid;
        }
    };

    // Transition table for player
    struct transition_table
        : mpl::vector<
              //    Start     Event         Next      Action Guard
              //  +---------+------------------+---------+---------------------------+----------------------+
              // stopped phase
              Row<Stopped, MyTurn, Playing, none, none>,
              Row<Stopped, GetExtortCard, Playing, none, IsExtortSrc>,
              // playing card phase
              Row<Playing, PlayCardSkip, Stopped, none, none>,
              Row<Playing, PlayCardShirk, Playing, none, none>,
              Row<Playing, PlayCardShirk, Stopped, none, IsShirkTarget>,
              Row<Playing, PlayCardReverse, Stopped, none, none>,
              Row<Playing, PlayCardSeeThrough, Playing, none, none>,
              Row<Playing, PlayCardPredict, Playing, none, none>,
              Row<Playing, PlayCardSwap, Playing, none, none>,
              Row<Playing, PlayCardShuffle, Playing, none, none>,
              // extort
              Row<Playing, PlayCardExtort, Stopped, none, none>,
              Row<Playing, PlayCardGetBottom, Stopped, none, none>,
              Row<Playing, PlayCardGetBottom, Exploding, none, IsBomb>,
              // draw card phase
              Row<Playing, DrawCard, Stopped, none, none>,
              Row<Playing, DrawCard, Exploding, none, IsBomb>,
              // extorted phase
              Row<Extorted, SelectExtortCard, Stopped, none, IsExtortTarget>,
              // exploding phase
              Row<Exploding, PlayCardBombDisposal, Stopped, none, none>,
              Row<Exploding, TimeOut, Died, none, none>
              //  +---------+-------------+---------+---------------------------+----------------------+
              > {};
    int Uid{0};
};