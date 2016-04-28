#ifndef __CORE_TASK_H__
#define __CORE_TASK_H__

#include "CoreBase.h"

NAMESPACE_BEG(core)

class CORE_CLASS Task
{
  public:
	virtual ~Task() {}
	virtual bool process() = 0;
};

NAMESPACE_END // namespace core

#endif // __CORE_TASK_H__
