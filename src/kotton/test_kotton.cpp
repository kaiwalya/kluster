
#include <iostream>
#include "kotton/kotton.hpp"

int main(int argc, const char * argv[])
{

	using namespace kotton;
	auto f = fiber::create([](){
		std::cout << "Hello World! - outer - entered" << std::endl;
		auto f = fiber::create([](){
			std::cout << "Hello World! - inner" << std::endl;
		});
		std::cout << "Hello World! - outer - starting inner" << std::endl;
		f->start();
		std::cout << "Hello World! - outer - exiting" << std::endl;
		delete f;
	});
	f->start();
	delete f;
}

