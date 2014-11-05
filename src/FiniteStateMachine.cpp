#include <iostream>
#include <assert.h>

// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>

#include <boost/thread.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;

using namespace std;

#define LOG(fmt, ...) {printf(fmt, ##__VA_ARGS__); printf("\n");}

namespace jz {
	// events
	// system online/log online event
	struct SystemOnlineEvent{};

	struct TransactionMachine_;
	typedef msm::back::state_machine<TransactionMachine_> TransactionStateMachine;
	// transaction received/ send transaction to server&log transaction information
	struct TransactionReceivedEvent{
		int timeout;  // in seconds
		msm::back::state_machine<TransactionMachine_> * machine;
		TransactionReceivedEvent(msm::back::state_machine<TransactionMachine_> * machine, int timeout=0): machine(machine), timeout(timeout){}
	};

	// transaction complete/ log transaction complete
	struct TransactionCompleteEvent{};

	// system error
	struct SystemErrorEvent{};

	// define FSM structure
	struct TransactionMachine_ : public msm::front::state_machine_def<TransactionMachine_> {
		template <class Event,class FSM>
		void on_entry(Event const& ,FSM&) { LOG( "entering transaction fsm" ) }
		template <class Event,class FSM>
		void on_exit(Event const&,FSM& ) { LOG( "leaving: transaction fsm" ) }

		//--- define states
		struct StartupState :public msm::front::state<> {
			enum {ID=0};
			template <class Event,class FSM>
			void on_entry(Event const&,FSM& ) { LOG( "entering: StartupState" )}
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) { LOG( "leaving: StartupState" )}
		};

		struct PendingState :public msm::front::state<> {
			enum {ID=1};
			template <class Event,class FSM>
			void on_entry(Event const&,FSM& ) { LOG( "entering: PendingState :%0x", uintptr_t(this) )}
			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) { LOG( "leaving: PendingState" ) }
		};
		struct TransactingState :public msm::front::state<> {
			enum {ID=2};
			typedef msm::back::state_machine<TransactionMachine_> TransactionStateMachine;
			typedef boost::shared_ptr<boost::thread> ThreadPtr;
			ThreadPtr timerThread;

			struct timer {
				int timeout; // milli second
				TransactionStateMachine& m;
				timer(TransactionStateMachine& m, int t): m(m),timeout(t) {
				}
				void operator()(){
					try{
						boost::this_thread::sleep( boost::posix_time::milliseconds(timeout));
						LOG( " Timeout to auoto fire SystemErrorEvent!" );
						m.process_event(SystemErrorEvent());
					}catch( boost::thread_interrupted& e) { // interrupted by transaction complete
					}
				}
			};

			template <class Event,class FSM>
			void on_entry(Event const& e,FSM& m) { 
				LOG( "entering: TransactingState" ) 
				timerThread.reset( new boost::thread(timer(*e.machine, e.timeout)) );
			}

			template <class Event,class FSM>
			void on_exit(Event const&,FSM& ) { LOG( "leaving: PendingState" ) }

			template <class FSM>
			void on_exit(TransactionCompleteEvent const&,FSM& ) { 
				timerThread->interrupt();
				if( !timerThread->timed_join(boost::posix_time::milliseconds(1000)) ) {
					LOG( "unable to interrupt timer!" );
				}else{
					LOG( "interrupted timer!" );
				}
				LOG( "leaving: TransactingState" ) 
			}
		};

		// define actions
		void LogOnline(const SystemOnlineEvent& s) { LOG("log system online") }
		void SendTransaction(const TransactionReceivedEvent& s) { LOG("send trasaction to server and log") }
		void LogTransactionComplete(const TransactionCompleteEvent& s) { LOG("log transaction complete") }


		typedef StartupState initial_state; // must define initial state
		typedef TransactionMachine_ M;

		// must define transition_table
		struct transition_table: mpl::vector<
			//         Start               Event                        Next                  Action				 Guard
			//    +------------------+--------------------------+------------------------+------------------------+----------------------+
			a_row < StartupState ,     SystemOnlineEvent,          PendingState ,           &M::LogOnline                                >,
			a_row < PendingState ,     TransactionReceivedEvent,   TransactingState,        &M::SendTransaction                          >,
			a_row < TransactingState,  TransactionCompleteEvent,   PendingState,            &M::LogTransactionComplete                   >,
			_row < TransactingState,  SystemErrorEvent,           StartupState                                                          >
		> {};

		static const char* StateName(int state) {
			static char const* const state_names[] = { "Startup", "Pending", "Transacting" };
			return state_names[state];
		}
		// Replaces the default no-transition response.
		template <class FSM,class Event>
		void no_transition(Event const& e, FSM&, int state) {
			LOG( "unexpected event %s on state %s", typeid(e).name(), StateName(state) );
		}

	}; // transaction_fsm_

}  // namespace jz

