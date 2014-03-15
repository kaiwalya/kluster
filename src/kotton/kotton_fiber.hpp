#if !defined(_IMPL_KOTTON_FIBER_HPP_)
#define _IMPL_KOTTON_FIBER_HPP_

#include <thread>
#include <list>

#include "kotton/fiber.hpp"

#include "kotton_execstack.hpp"

namespace kotton {
		
	struct fiber_base: fiber {
		fiber_base(fiber_base * parent, userfunc & f): mParent(parent), mExec(f, mStack), mYieldReason(0) {}
		~fiber_base();
		const exec_state & state() { return mExec.state();}
		void start() override;
		static fiber_base * current();
		/**
			Jump to this fiber_base. Returns true if there is still processing remaining.
		*/
		virtual bool proceed();
		
		/**
			Jump back to whoever called proceed
		*/
		virtual void yield();
		
		/**
			Set Resons for yield
		*/
		virtual int64_t & yieldReason() {return mYieldReason;}
	private:
		stack mStack;
		execution mExec;
		int64_t mYieldReason;
		std::list<fiber_base *> mBlockedToFinish;
		fiber_base * mParent;
		fiber_base * mRoot;
	};

	/**Combines a fiber and a executing thread*/
	struct thread: fiber_base {
	private:
		/**Find the current thread from tls*/
		static thread * & current();
		
	public:
		/**Find the current fiber*/
		static fiber_base * currentFiber() {return current()->mCurrentFiber;}
	
	private:
		void schedule();
		static void _schedule() {current()->schedule();}
		static userfunc scheduleCaller;
	public:
		/**Create new thread or fiber_base*/
		static fiber * create(userfunc & f);
		
	private:
		/**Private constructor, called form static create if required*/
		thread(userfunc & f): fiber_base(nullptr, scheduleCaller), mThread(), mCurrentFiber(nullptr) {
			mFibers.push_back(new fiber_base(this, f));
		}
		
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
		std::list<fiber_base *> mFibers;
		fiber_base * mCurrentFiber;
	};

}

#endif
