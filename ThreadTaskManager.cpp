#include "ThreadTaskManager.hpp"

#include <QFuture>

namespace Qaos {
	void ThreadTask::run()
	{
	}

	void ThreadTask::abort()
	{
		_promise.future().cancel();
	}

	bool ThreadTask::isAborted() const
	{
		return _promise.isCanceled();
	}

	ThreadTaskManager* ThreadTaskManager::GlobalInstance()
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
		_task->_promise.start();
		emit _task->started();
		_task->run();
		if (_task->isAborted()) {
			emit _task->aborted();
		} else {
			emit _task->finished();
		}
		_task->_promise.finish();
	}

	const ThreadTask& ThreadTaskManager::Container::get() const
	{
		return *_task;
	}
}
