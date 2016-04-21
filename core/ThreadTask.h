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
		TPTask_Completed			 = 0, // һ�������Ѿ����
		TPTask_ContinueMainThread	 = 1, // ���������߳�ִ��
		TPTask_ContinueChildThread	 = 2, // ���������߳�ִ��
	};

	virtual TPTask::TPTaskState presentMainThread() { return TPTask::TPTask_Completed; }
};

NAMESPACE_END // namespace core

#endif // __CORE_THREADTASK_H__
