#include "FiniteStateMachine.h"

int currentStateID(jz::TransactionStateMachine& m) { return m.current_state()[0]; }

void print(jz::TransactionStateMachine& m) {
	LOG("current state: %s", jz::TransactionMachine_::StateName(m.current_state()[0]))
}

void test_fsm(){
	using namespace jz;
	TransactionStateMachine m;
	TransactionReceivedEvent transactionReceived(&m, 3000);  // transanction is excepted to complete in 3 seconds. (For testing purpose, the required 30 seconds in question is reduced to 3 seconds)
	m.start();
	assert (currentStateID(m) == TransactionMachine_::StartupState::ID);
	// test unexpected event
	m.process_event(transactionReceived);             // no trasition
	assert (currentStateID(m) == TransactionMachine_::StartupState::ID);

	// start 
	m.process_event(SystemOnlineEvent());             // to Pending
	assert (currentStateID(m) == TransactionMachine_::PendingState::ID);

	m.process_event(transactionReceived);             // to Transacting
	assert (currentStateID(m) == TransactionMachine_::TransactingState::ID);

	boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	m.process_event(TransactionCompleteEvent());      // to Pending
	assert (currentStateID(m) == TransactionMachine_::PendingState::ID);

	m.process_event(transactionReceived);              // to Transacting
	assert (currentStateID(m) == TransactionMachine_::TransactingState::ID);

	boost::this_thread::sleep(boost::posix_time::milliseconds(5000));  // what for timeout. now it auto triggers SystemErrorEvent, to Startup
	assert (currentStateID(m) == TransactionMachine_::StartupState::ID);
	m.process_event(SystemErrorEvent());              // no trasition Startup


	print(m);

	m.stop();
}
int main(int argc, char* argv[])
{
	test_fsm();
	return 0;
}