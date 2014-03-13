
#include <iostream>
#include <exception>
#include <assert.h>
#include <stdio.h>
#include <memory>
#include <chrono>

#include "kotton/kotton.hpp"

namespace kotton {
	struct thread;
}

static __thread kotton::thread * __this_thread = nullptr;

namespace kotton {

	struct thread {
		static thread * current();
		static thread * create(fiber_info * f);
		
		void scheduleFiber(fiber_info * f);
		std::thread mThread;
		fiber_info * mRootInfo;
		fiber_info * mCurrentInfo;
	};
	
	
	thread * thread::current() {
		return __this_thread;
	}
	
	thread * thread::create(fiber_info * info) {
		thread * ret = new thread();
		

		std::mutex m;
		std::condition_variable c;
		bool initDone = false;
		auto worker = [ret, info, &m, &c, &initDone](){
			{
				std::unique_lock<std::mutex> l(m);
				__this_thread = ret;

				ret->mRootInfo = info;
				ret->mCurrentInfo = info;
				
				initDone = true;
				c.notify_all();
			}
			
			info->mFunc();
			
			__this_thread = nullptr;
			ret->mThread.detach();
			delete ret;
		};
		
		{
			std::unique_lock<std::mutex> l(m);
			std::thread t(worker);
			ret->mThread.swap(t);
			assert(!ret->mCurrentInfo);
			while(!initDone) {
				c.wait(l);
			}
			assert(ret->mCurrentInfo);
		}
		
		return ret;
	}
	
	void thread::scheduleFiber(fiber_info *f) {
		auto temp = mCurrentInfo;
		mCurrentInfo = f;
		f->mFunc();
		mCurrentInfo = temp;
	}


	fiber::fiber(userfunc & f) {
		auto currThread = thread::current();
		if (currThread) {
			mInfo = std::unique_ptr<fiber_info>(new fiber_info(currThread->mCurrentInfo, f));
		}
		else {
			mInfo = std::unique_ptr<fiber_info>(new fiber_info(f));
		}
	}
	
	
	void fiber::start() {
		auto currThread = thread::current();
		if (currThread) {
			currThread->scheduleFiber(mInfo.get());
		}
		else {
			thread::create(mInfo.get());
		}
	}
	
}

