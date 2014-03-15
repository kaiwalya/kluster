#if !defined(_IMPL_KOTTON_FIBER_HPP_)
#define _IMPL_KOTTON_FIBER_HPP_

#include <thread>
#include <list>

#include "kotton/fiber.hpp"

#include "kotton_execstack.hpp"

namespace kotton {

	struct thread;
		
	struct fiber_base: fiber {
		fiber_base(fiber_base * parent, userfunc & f): mParent(parent), mExec(f, mStack) {}

		~fiber_base();
		
		const exec_state & state() { return mExec.state();}
		void start() override;

		/**
			Jump to this fiber_base. Returns true if there is still processing remaining.
		*/
		virtual bool proceed();
		
		
		void setThread(thread * root) {mRoot = root;}
		
		/**
			Jump back to whoever called proceed
		*/
		virtual void yield();
		
	private:
		stack mStack;
		execution mExec;
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
		static std::shared_ptr<fiber> create(userfunc & f);
		
	private:
		/**Private constructor, called form static create if required*/
		thread(userfunc & f): fiber_base(nullptr, scheduleCaller), mThread(), mCurrentFiber(nullptr) {
			fiber_base::setThread(this);
			auto first = std::make_shared<fiber_base>(this, f);
			first->setThread(this);
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
		
	public:
		void onKill(fiber_base * who);
		
	private:
		mutex m;
		std::thread mThread;
		std::list<std::shared_ptr<fiber_base>> mFibers;
		fiber_base * mCurrentFiber;
		
	};

}

#endif
