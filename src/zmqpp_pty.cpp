//============================================================================
// Name        : zmqpp_pty.cpp
// Author      : KS4Nguyen
// Version     :
// Copyright   : MPL v2.0
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include <thread>

using namespace std;

void hello( int limit ) {
	int loop = limit;
	while ( loop++ < 3 ) {
		cout << "Hello World. (" << loop << ")"<< endl;
	}	
}

int main() {
	const int limit = 3;
	thread t( hello, limit ); 

	t.join();
}


