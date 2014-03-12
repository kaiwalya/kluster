
#include <iostream>
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

