#include "TaskManager.h"

NAMESPACE_BEG(core)

TaskManager::TaskManager() : mTasks()
{
}

TaskManager::~TaskManager()
{
}

void TaskManager::add(Task *pTask)
{
	mTasks.push_back(pTask);
}

bool TaskManager::cancel(Task *pTask)
{
	Container::iterator iter = std::find(mTasks.begin(), mTasks.end(), pTask);
	if (iter != mTasks.end())
	{
		mTasks.erase(iter);
		return true;
	}

	return false;
}

void TaskManager::process()
{
	Container::iterator iter = mTasks.begin();

	while (iter != mTasks.end())
	{
		Task *pTask = *iter;
		if(!pTask->process())
			iter = mTasks.erase(iter);
		else
			++iter;
	}
}

NAMESPACE_END // namespace core
