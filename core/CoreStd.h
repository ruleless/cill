#ifndef __CORE_COREBASE_H__
#define __CORE_COREBASE_H__

#include "CoreBase.h"

NAMESPACE_BEG(core)

void sleepms(ulong ms);

// 获得系统产生的最后一次错误描述
char* coreStrError(int ierrorno = 0);
int coreLastError();

NAMESPACE_END // namespace core

#endif // __CORE_COREBASE_H__
