#ifndef __CORE_TASKMANAGER_H__
#define __CORE_TASKMANAGER_H__

#include "Task.h"

NAMESPACE_BEG(core)

class TaskManager
{
  public:
	TaskManager();
	~TaskManager();

	void add(Task *pTask);
	bool cancel(Task *pTask);
	void process();
  private:
	typedef std::vector<Task *> Container;
	Container mTasks;
};

NAMESPACE_END // namespace core

#endif // __CORE_TASKMANAGER_H__