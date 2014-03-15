#include <assert.h>

#include "kotton_fiber.hpp"

/**TLS which stores the root fiber*/
static __thread kotton::thread * __this_thread = nullptr;

namespace kotton {
	std::shared_ptr<fiber> fiber::create(userfunc &f) {
		return thread::create(f);
	}
	
	fiber_base::~fiber_base() {
		if (state() != exec_state::finished) {
			mRoot->onKill(this);
		}
	}
	
	void fiber_base::start() {
		/**
			We have permission to start, but let the schedular choose
			Note, we use current because we want to return the control from current fiber
			back to the schedular.
		*/
		mRoot->currentFiber()->yield();
	}
	
	bool fiber_base::proceed() {
		return mExec.proceed();
	}
	
	void fiber_base::yield() {
		mExec.yield();
	};

	std::shared_ptr<fiber> thread::create(userfunc & f) {
		thread * curr = current();
		if (curr) {
			/**Create child fiber*/
			assert(curr->mCurrentFiber);
			auto ret = std::make_shared<fiber_base>(curr->mCurrentFiber, f);
			ret->setThread(curr);
			curr->mFibers.push_back(ret);
			return ret;
		}
		else {
			return std::shared_ptr<thread>(new thread(f));
		}
	}
	
	thread * & thread::current() {
		return __this_thread;
	}
	
	userfunc thread::scheduleCaller = [](){thread::_schedule();};
	void thread::schedule() {
		
		while (mFibers.size()) {
			auto fRefChoice = mFibers.front();
			auto fChoice = fRefChoice.get();
			assert(fChoice->state() == exec_state::paused);
			
			mCurrentFiber = fChoice;
			bool proceed = fChoice->proceed();
			assert(mCurrentFiber == fChoice);
			mCurrentFiber = this;
			
			if (!proceed) {
				assert(fChoice->state() == exec_state::finished);
				/**Returning from somewhere else*/
				if (fChoice->state() == exec_state::finished) {
					mFibers.remove(fRefChoice);
				}
			}
		}
		
		/* The schedular should never return when there are fibers to schedule */
		assert(!mFibers.size());
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
				assert(t->mCurrentFiber == t);
				assert(!t->mFibers.size());
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
	
	void thread::onKill(kotton::fiber_base *target) {
		//Is this leagal? If yes, we can cleanup here, we will have to do it in the schedular
		assert(target != mCurrentFiber);
		target->setThread(nullptr);
		mFibers.remove_if([target](const std::shared_ptr<fiber_base> & ref){
			return target == ref.get();
		});
	}
	
	thread::~thread() {
		if (mThread.joinable())
			mThread.join();
	}
}