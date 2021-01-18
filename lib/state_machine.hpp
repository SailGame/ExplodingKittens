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
#define BombNum 4
#define SkipNum 8
#define ShirkNum 5
#define ReverseNum 5
#define PredictNum 4
#define SeeThroughNum 4
#define SwapNum 3
#define GetBottomNum 3
#define ShuffleNum 4
#define ExtortNum 4
#define BombDisposalNum 6
#define InitialCardsNum 5
namespace mpl = boost::mpl;
namespace msm = boost::msm;
using namespace msm::front;
using namespace msm::front::euml;
enum CardType {
    Bomb,
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
static std::vector<std::string> CardNames = {
    "Bomb", "Skip",      "Shirk",   "Reverse", "Predict",     "SeeThrough",
    "Swap", "GetBottom", "Shuffle", "Extort",  "BombDisposal"};
struct Player;
struct MyTurn;
struct NextTurn {};
struct GameStart {};
struct Game : public msm::front::state_machine_def<Player> {
    Game(std::vector<int> uids) {
        std::map<CardType, int> Cards{
            {Skip, SkipNum},
            {Shirk, ShirkNum},
            {Reverse, ReverseNum},
            {Predict, PredictNum},
            {SeeThrough, SeeThroughNum},
            {Swap, SwapNum},
            {GetBottom, GetBottomNum},
            {Shuffle, ShuffleNum},
            {Extort, ExtortNum},
            {BombDisposal, BombDisposal - BombNum - 1}};

        for (auto it : Cards) {
            for (int i = 0; i < it.second; ++i)
                mCardPool.emplace_back(it.first);
        }
        random_shuffle(mCardPool.begin(), mCardPool.end());
        for (auto uid : uids) {
            std::vector<CardType> tmpvec(
                mCardPool.begin(), mCardPool.begin() + InitialCardsNum - 1);
            tmpvec.push_back(BombDisposal);

            mCardPool.erase(mCardPool.begin(),
                            mCardPool.begin() + InitialCardsNum - 1);
            mPlayers.emplace_back(uid, tmpvec, *this);
        }
        for (int i = 0; i < BombNum; ++i) {
            mCardPool.push_back(Bomb);
        }
        random_shuffle(mCardPool.begin(), mCardPool.end());
    }

    // entry and exit of state: log
    template <class Event, class FSM>
    void on_entry(Event const&, FSM&) {
        std::cout << "entering: Player" << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM&) {
        std::cout << "leaving: Player" << std::endl;
    }

    struct Prepared : public msm::front::state<> {};

    struct Playing : public msm::front::state<> {};

    struct Ended : public msm::front::state<> {};

    // the initial state of the player SM. Must be defined
    typedef Prepared initial_state;
    // transition actions
    struct FirstPlayer {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            fsm.PlayingPlayer = fsm.mPlayers.begin();
            fsm.PlayingPlayer->process_event(MyTurn());
        }
    };
    struct NextPlayer {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            if (fsm.mClockwise)
                fsm.PlayingPlayer = fsm.PlayingPlayer++;
            else
                fsm.PlayingPlayer = fsm.PlayingPlayer--;
            fsm.PlayingPlayer->process_event(MyTurn());
        }
    };
    // guards
    struct OnlyOnePlayerLeft {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src,
                        TargetState& tgt) {
            return fsm.mPlayers.size() == 1;
        }
    };
    // Transition table for player
    struct transition_table
        : mpl::vector<
              //    Start     Event         Next      Action Guard
              //  +---------+------------------+---------+---------------------------+----------------------+
              // stopped phase
              Row<Prepared, GameStart, Playing, FirstPlayer, none>,
              Row<Playing, NextTurn, Playing, none, none>,
              Row<Playing, NextTurn, Ended, none, OnlyOnePlayerLeft>
              //  +---------+-------------+---------+---------------------------+----------------------+
              > {};

    std::list<Player> mPlayers;
    std::list<Player> mDiedPlayers;
    bool mClockwise{true};
    std::list<Player>::iterator PlayingPlayer;
    std::vector<CardType> mCardPool;
    void PrintCardPoool() {
        std::cout << "Game Card Pool\n";
        for (auto it : mCardPool) {
            std::cout << CardNames[it] << " ";
        }
        std::cout << std::endl;
    }
};

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

struct Player : public msm::front::state_machine_def<Player> {
    Player(int uid, std::vector<CardType> cards, Game& game)
        : mUid(uid), mCards(cards), mGame(game) {}

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
    struct EndOfTurn {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            auto pos =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(pos);
            fsm.mGame.process_event(NextTurn());
        }
    };

    struct Reverse {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            auto pos =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(pos);
            fsm.mGame.turn = !fsm.mGame.turn;
            fsm.mGame.process_event(NextTurn());
        }
    };

    struct SeeThrough {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            auto pos =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(pos);
            // TODO: see through
        }
    };

    struct Predict {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            auto pos =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(pos);
            // TODO: predict
        }
    };

    struct Swap {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            auto pos =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(pos);
            for (auto player : fsm.mGame.mPlayers) {
                if (player.mUid == evt.TargetUid)
                    swap(fsm.mCards, player.mCards);
            }
        }
    };

    struct Shuffle {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            auto pos =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(pos);
            random_shuffle(fsm.mGame.mCardPool.begin(),
                           fsm.mGame.mCardPool.end());
        }
    };

    struct HandleExploding {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            auto pos =
                std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Card);
            fsm.mCards.erase(pos);
            // TODO: ask user where to put bomb
            // handle user respond;
        }
    };

    struct StoreCard {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            fsm.mCards.push_back(evt.Card);
        }
    };

    struct SelectExtortTarget {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            for (auto player : fsm.mGame.mPlayers) {
                if (player.mUid == evt.TargetUid)
                    player.process_on(AskExtortCard(fsm.mUid));
            }
        }
    };

    struct AskExtortCard {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState&,
                        TargetState& tstate) {
            tstate.ExtortSrcId = evt.ExtortSrcUid;
            // TODO: ask user to give an card
        }
    };

    struct ExtortCardSelected {
        template <class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& sstate,
                        TargetState& tstate) {
            for (auto player : fsm.mGame.mPlayers) {
                if (player.mUid == sstate.ExtortSrcUid) {
                    player.process_event(GetExtortCard(evt.Card));
                }
            }
        }
    };

    // guard conditions
    struct DoesCardExist {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            return std::find(fsm.mCards.begin(), fsm.mCards.end(), evt.Cards) !=
                   fsm.mCards.end();
        }
    };

    struct IsShirkTarget {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            return evt.TatgetUid == fsm.mUid;
        }
    };
    struct IsBomb {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            return evt.Card == Bomb;
        }
    };
    struct IsExtortTarget {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            return evt.TatgetUid == fsm.mUid;
        }
    };
    struct IsTargetAlive {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState&, TargetState&) {
            for (auto player : fsm.mGame.mPlayers) {
                if (player.mUid == evt.TargetUid) return true;
            }
            return false;
        }
    };

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
              > {};
    int mUid{0};
    std::vector<CardType> mCards;
    void PrintCards() {
        std::cout << "Player " << mUid << " own " << mCards.size()
                  << " cards:\n";
        for (auto it : mCards) {
            std::cout << CardNames[it] << " ";
        }
        std::cout << std::endl;
    }
    Game& mGame;
};