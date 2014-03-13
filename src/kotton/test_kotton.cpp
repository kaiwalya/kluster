
#include <iostream>
#include "kotton/kotton.hpp"

int main(int argc, const char * argv[])
{

	using namespace kotton;
	
	auto root = newFiber("root", [](){
		newFiber("inner", [] () {
			printf("Hello ");
		})->start();
		printf(" world!\n");
	});

	root->start();
	std::this_thread::sleep_for((std::chrono::seconds)1);
}

