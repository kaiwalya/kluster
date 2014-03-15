#if !defined(_IMPL_KOTTON_FIBER_HPP_)
#define _IMPL_KOTTON_FIBER_HPP_

#include <thread>
#include <list>

#include "kotton/fiber.hpp"

#include "kotton_execstack.hpp"

namespace kotton {

	struct thread;
		
	struct fiber_base: fiber {
		fiber_base(fiber_base * parent, userfunc & f): mParent(parent), mExec(f, mStack), mYieldReason(0) {}
		~fiber_base();
		const exec_state & state() { return mExec.state();}
		void start() override;

		/**
			Jump to this fiber_base. Returns true if there is still processing remaining.
		*/
		virtual bool proceed();
		
		void attach(thread * root) {mRoot = root;}
		
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
		thread * mRoot;
	};

	/**Combines a fiber and a executing thread*/
	struct thread: fiber_base {
	private:
		/**Find the current thread from tls*/
		static thread * & current();
		
	public:
		/**Find the current fiber*/
		fiber_base * currentFiber() {return mCurrentFiber;}
	
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
			fiber_base::attach(this);
			auto first = new fiber_base(this, f);
			first->attach(this);
			mFibers.push_back(first);
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
