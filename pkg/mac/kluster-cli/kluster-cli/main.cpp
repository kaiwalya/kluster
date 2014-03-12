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
	using namespace kotton;
	create()->delegate("main", [](){
		auto subscribe = self()->delegate("main", [](){
			self()->become([](){
				self()->unbecome();
			});
		});
	
		self()->become([](){
			self()->nextMessage();
			self()->unbecome();
		});
		
		self()->unbecome();
	});
	
	
}

