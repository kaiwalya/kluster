#include <assert.h>

#include "kotton_fiber.hpp"

/**TLS which stores the root fiber*/
static __thread kotton::thread * __this_thread = nullptr;

namespace kotton {
	fiber * fiber::create(userfunc &f) {
		return thread::create(f);
	}
	
	fiber_base * fiber_base::current() {
		return thread::currentFiber();
	};
	
	fiber_base::~fiber_base() {
	}
	
	void fiber_base::start() {
		/**
			We have permission to start, but let the schedular choose
			Note, we use current because we want to return the control from current fiber
			back to the schedular.
		*/
		current()->yield();
	}
	
	bool fiber_base::proceed() {
		return mExec.proceed();
	}
	
	void fiber_base::yield() {
		mExec.yield();
	};

	fiber * thread::create(userfunc & f) {
		thread * curr = current();
		if (curr) {
			/**Create child fiber*/
			assert(curr->mCurrentFiber);
			fiber_base * ret = new fiber_base(curr->mCurrentFiber, f);
			ret->attach(curr);
			curr->mFibers.push_back(ret);
			return ret;
		}
		else {
			return new thread(f);
		}
	}
	
	thread * & thread::current() {
		return __this_thread;
	}
	
	userfunc thread::scheduleCaller = [](){thread::_schedule();};
	void thread::schedule() {
		
		while (mFibers.size()) {
			mCurrentFiber = mFibers.front();
			assert(mCurrentFiber->state() == exec_state::paused);
			if (!mCurrentFiber->proceed()) {
				assert(mCurrentFiber->state() == exec_state::finished);
				/**Returning from somewhere else*/
				if (mCurrentFiber->state() == exec_state::finished) {
					mFibers.remove(mCurrentFiber);
				}
			}
		}
	}
	
	void thread::start() {
		
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
				
				/* The schedular should never return when there are fibers to schedule */
				assert(t->mCurrentFiber != t && t->mFibers.size());
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