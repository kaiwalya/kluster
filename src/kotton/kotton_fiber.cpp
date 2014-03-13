#include <assert.h>

#include "kotton_fiber.hpp"

/**TLS which stores the root fiber*/
static __thread kotton::thread * __this_thread = nullptr;

namespace kotton {
	fiber * fiber::create(userfunc &f) {
		return thread::create(f);
	}
	
	void fiber_base::proceed() {
		mExec.proceed();
	}

	fiber * thread::create(userfunc & f) {
		thread * curr = current();
		if (curr) {
			/**Create child fiber*/
			assert(curr->mCurrentFiber);
			return new fiber_base(curr->mCurrentFiber, f);
		}
		else {
			return new thread(f);
		}
	}
	
	thread * & thread::current() {
		return __this_thread;
	}
	
	void thread::proceed() {
		
		bool initDone;
		thread::condition c;
		auto worker = [&initDone, &c](thread * t){
			{
				auto l = t->lock();
				current() = t;
				t->mCurrentFiber = t;
				initDone = true;
				c.notify_all();
			}
			{
				
				t->fiber_base::proceed();
			}
			
			{
				auto l = t->lock();
				t->mCurrentFiber = nullptr;
				current() = nullptr;
			}
		};
		
		{
			auto l = lock();
			initDone = false;
			std::thread t(worker, this);
			mThread.swap(t);
			/**Wait for worker to set this to true*/
			while(initDone == false) {
				c.wait(l);
			}
			assert(mThread.joinable());
		}
	}
	
	thread::~thread() {
		if (mThread.joinable())
			mThread.join();
	}
}