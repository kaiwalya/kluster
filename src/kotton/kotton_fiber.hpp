#if !defined(_IMPL_KOTTON_FIBER_HPP_)
#define _IMPL_KOTTON_FIBER_HPP_

#include <thread>

#include "kotton/fiber.hpp"

namespace kotton {
  
	struct fiber_base: fiber {
		fiber_base(fiber_base * parent, userfunc & f): mParent(parent), mFunc(f) {}
		
		void start() override;
	private:
		userfunc mFunc;
		fiber_base * mParent;
	};

	/**Combines a fiber and a executing thread*/
	struct thread: fiber_base {
	private:
		/**Find the current thread from tls*/
		static thread * & current();
		
	public:
		/**Create new thread or fiber_base*/
		static fiber * create(userfunc & f);
	private:
		/**Private constructor, called form static create if required*/
		thread(userfunc & f): fiber_base(nullptr, f), mThread(), mCurrentFiber(nullptr) {}
		
	public:
		using mutex = std::mutex;
		using condition = std::condition_variable;
		using locker = std::unique_lock<mutex>;
		
		locker lock() {return locker(m);}
		
		/**fiber_base override. Stars a new thread, calls base class start on the new thread*/
		void start() override;
		
		~thread();

	private:
		mutex m;
		std::thread mThread;
		
		fiber_base * mCurrentFiber;
	};

}

#endif
