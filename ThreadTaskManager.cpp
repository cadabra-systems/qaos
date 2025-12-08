#include "ThreadTaskManager.hpp"

namespace Qaos {
	void ThreadTask::run()
	{
	}

	ThreadTaskManager* ThreadTaskManager::Instance()
	{
		static ThreadTaskManager retval(QThreadPool::globalInstance());
		return &retval;
	}

	ThreadTaskManager::ThreadTaskManager(QThreadPool* thread_pool)
	:
		_thread_pool(thread_pool ? thread_pool : new QThreadPool(this))
	{

	}

	ThreadTaskManager::~ThreadTaskManager()
	{

	}

	void ThreadTaskManager::run(ThreadTask* task)
	{
		_thread_pool->start(new ThreadTaskManager::Container(task));
	}

	ThreadTaskManager::Container::Container(ThreadTask* task)
	:
		_task(task ? task : new ThreadTask())
	{

	}

	ThreadTaskManager::Container::~Container()
	{
		_task->deleteLater();
	}

	void ThreadTaskManager::Container::run()
	{
		emit _task->started();
		/// @todo aborted case
		_task->run();
		emit _task->finished();
	}

	const ThreadTask& ThreadTaskManager::Container::get() const
	{
		return *_task;
	}
}
