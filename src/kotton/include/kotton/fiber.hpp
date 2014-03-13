#if !defined(_KOTTON_FIBER_H)
#define _KOTTON_FIBER_H

/**
	fibers - used for non pre-emptive, single threaded context switching between function objects
	
	* function objects dont take parameters because if we added extension,
	we will need to change parameters, so each extension might define its
	own function type, which means problems, especially using multiple extensions
	
	Extensions
		* Support for data exchange between fibers
		* Support for learning behaviours of data exchange (topics, pub-sub)
		* Add support to spawn multiple fiber systems if behaviour demands it
		* Add support for function registries, so functions can be looked up and run remotely as required

*/

#include <exception>

#include "kotton/userfunc.hpp"

namespace kotton {

	struct fiber;
	
	struct fiber {
		static fiber * create(userfunc & f);
		static fiber * create(userfunc && f) {return create(f);}
		virtual ~fiber() {};
		virtual void proceed() = 0;
	};
	
	struct err_not_implemented: std::exception {
		const char * what() const noexcept override{
			return "This feature is not implemented";
		}
	};
}

#endif