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
	using name = std::string;
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
		fiber_info(name &n, userfunc & f): mName(n), mFunc(f), mParent(nullptr) {}
		
		/**Create a child fiber_info*/
		fiber_info * newFiber(name &n, userfunc & f) {
			return new fiber_info(this, n, f);
		}
		
		fiber_info(fiber_info * parent, name &n, userfunc & f): mName(n), mFunc(f), mParent(parent) {}
		name mName;
		userfunc mFunc;
		fiber_info * mParent;
		
	};
	
	struct fiber {
		fiber(name & n, userfunc & f);
		fiber(name & n, userfunc && f) : fiber(n, f) {}
		fiber(name && n, userfunc & f) : fiber(n, f) {}
		fiber(name && n, userfunc && f) : fiber(n, f) {}
		
		Subscription subscribe(topic & t);
		Subscription subscribe(topic && t) {return subscribe(t);}
		
		void start();
	private:
		std::unique_ptr<fiber_info> mInfo;
	};
	using Fiber = std::shared_ptr<fiber>;
	
	inline Fiber newFiber(name & n, userfunc & f) {
		return Fiber(new fiber(n,f));
	}
	
	inline Fiber newFiber(name & n, userfunc && f) {
		return newFiber(n,f);
	}
	inline Fiber newFiber(name && n, userfunc & f) {
		return newFiber(n,f);
	}
	inline Fiber newFiber(name && n, userfunc && f) {
		return newFiber(n,f);
	}
	
	struct err_not_implemented: std::exception {
		const char * what() const noexcept override{
			return "This feature is not implemented";
		}
	};
}

#endif

