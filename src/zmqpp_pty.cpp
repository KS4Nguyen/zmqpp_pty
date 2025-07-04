//============================================================================
// Name        : zmqpp_pty.cpp
// Author      : KS4Nguyen
// Version     :
// Copyright   : MPL v2.0
// Description : Hello World in C++ (Thread)
//============================================================================

#include <iostream>

#include <thread>
#include <atomic>

#include <chrono>

using namespace std;

atomic<int> msg_cnt;
atomic<bool> hello_running;

void hello( int threshold, string *buff ) {
	int loop = threshold;
	while ( loop-- > 0 ) {
		this_thread::sleep_for( chrono::milliseconds(50) );
		*buff = "Hello World. (" + to_string(loop) + ")";
	}
	hello_running = 0;
}

int main() {
	int limit = 3;
    string msg_buff, msg_buff_old = "\0";
    
    msg_buff = "Looping... (" + to_string(limit) + ")";
    cout << msg_buff << endl;
    msg_buff = "\0";
    
    hello_running = 1;
	thread t( hello, limit, &msg_buff );

	while ( hello_running ) {
		if ( msg_buff != msg_buff_old ) {
			cout << msg_buff << endl;
			msg_buff_old = msg_buff;
		}
	}

	t.join();
	return 0;
}


