//
//  main.cpp
//  kluster-cli
//
//  Created by Kaiwalya Kher on 3/9/14.
//  Copyright (c) 2014 Kaiwalya Kher. All rights reserved.
//

#include <iostream>
#include <thread>
#include <assert.h>
#include "kotton/kotton.hpp"

int main(int argc, const char * argv[])
{
	//using namespace kotton;
	//wait(make([](){
		
		struct print: kotton::message {
			print(std::string msg): msg(msg) {}
			std::string msg;
		};
		{
			auto self = kotton::self();
			self->send(print("Hello"));
			self->send(print(" "));
			self->send(print("World"));
			self->send(print("!"));
			self->send(print("\n"));
			self->send(kotton::stop_message());
		}
		
		for(;;) {
			auto & msg = kotton::me()->top();
			if (msg.isA<kotton::stop_message>()) {
				break;
			}
			else if (msg.isA<print>()) {
				std::cout << msg.asA<print>().msg;
			}
			else {
				throw std::exception();
			}
		}
	//}));
}

