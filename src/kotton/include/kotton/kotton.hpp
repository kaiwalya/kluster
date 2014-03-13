#if !defined(_KOTTON_H_)
#define _KOTTON_H_

#include <memory>
#include <functional>
#include <stack>
#include <exception>
#include <thread>
#include <typeinfo>
#include <typeindex>

namespace kotton {
	using userfunc = std::function<void(void)>;
	using topic = std::string;
		
	struct fiber;
	struct fiber_info;
	struct thread;
		
	struct value {
		value() = delete;
		value(const value &) = delete;
		value& operator = (const value&) = delete;
	};
	
	struct subscription {
		value & pop();
	};
	
	using Subscription = std::shared_ptr<subscription>;
	
	struct fiber_info {
		/**Create root level fiber_info*/
		fiber_info(userfunc & f): mFunc(f), mParent(nullptr) {}
		
		/**Create a child fiber_info*/
		fiber_info(fiber_info * parent, userfunc & f): mFunc(f), mParent(parent) {}

		userfunc mFunc;
		fiber_info * mParent;
		
	};
	
	struct fiber {
		fiber(userfunc & f);
		fiber(userfunc && f) : fiber(f) {}
		
		Subscription subscribe(topic & t);
		Subscription subscribe(topic && t) {return subscribe(t);}
		
		void start();
	private:
		std::unique_ptr<fiber_info> mInfo;
	};
	using Fiber = std::shared_ptr<fiber>;
	
	inline Fiber newFiber(userfunc & f) {
		return Fiber(new fiber(f));
	}
	
	inline Fiber newFiber(userfunc && f) {
		return newFiber(f);
	}
	struct err_not_implemented: std::exception {
		const char * what() const noexcept override{
			return "This feature is not implemented";
		}
	};
}

#endif

