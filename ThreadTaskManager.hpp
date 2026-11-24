#ifndef Qaos_ThreadTaskManager_hpp
#define Qaos_ThreadTaskManager_hpp

#include <QObject>
#include <QPromise>

#include <QThreadPool>
#include <QRunnable>

namespace Qaos {
	class ThreadTask : public QObject
	{
	friend class ThreadTaskManager;

	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		ThreadTask() = default;
		ThreadTask(ThreadTask&&) = delete;
		ThreadTask(const ThreadTask&) = delete;
		virtual ~ThreadTask() override = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		ThreadTask& operator()(ThreadTask&&) = delete;
		ThreadTask& operator()(const ThreadTask&) = delete;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		QPromise<void> _promise;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual void run();
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void started();
		void aborted();
		void finished();
	/** @} */

	/** @name Slots */
	/** @{ */
	public slots:
		void abort();
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isAborted() const;
	/** @} */
	};

	class ThreadTaskManager : public QObject
	{
	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static ThreadTaskManager* GlobalInstance();
	/** @} */

	/** @name Classes */
	/** @{ */
	private:
		class Container : public QRunnable
		{
		public:
		/** @name Constructors */
		/** @{ */
			Container(ThreadTask* task);
			virtual ~Container() override;
		/** @} */

		/** @name Properties */
		/** @{ */
		private:
			ThreadTask* const _task;
		/** @} */

		/** @name Procedures */
		/** @{ */
		public:
			virtual void run() override;
		/** @} */

		/** @name Getters */
		/** @{ */
		public:
			const ThreadTask& get() const;
		/** @} */
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		ThreadTaskManager(QThreadPool* thread_pool = nullptr);
		virtual ~ThreadTaskManager() override;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		QThreadPool* const _thread_pool;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void run(ThreadTask* task);
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void run();
	/** @} */
	};
}

#endif
