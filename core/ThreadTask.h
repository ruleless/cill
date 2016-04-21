#ifndef __CORE_THREADTASK_H__
#define __CORE_THREADTASK_H__

#include "CoreBase.h"
#include "Task.h"

NAMESPACE_BEG(core)

class TPTask : public Task
{
  public:
	enum TPTaskState
	{
		TPTask_Completed			 = 0, // 一个任务已经完成
		TPTask_ContinueMainThread	 = 1, // 继续在主线程执行
		TPTask_ContinueChildThread	 = 2, // 继续在子线程执行
	};

	virtual TPTask::TPTaskState presentMainThread() { return TPTask::TPTask_Completed; }
};

NAMESPACE_END // namespace core

#endif // __CORE_THREADTASK_H__
