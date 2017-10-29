#ifndef __CMDBASE_H__
#define __CMDBASE_H__

#include "core/cillcore.h"

#ifdef _CMD_DLL_EXPORT
#define CMD_CLASS       CLASS_EXPORT
#define CMD_API         API_EXPORT
#define CMD_VAR         VAR_EXPORT
#else
#define CMD_CLASS       CLASS_IMPORT
#define CMD_API         API_IMPORT
#define CMD_VAR         VAR_IMPORT
#endif

#endif // __CMDBASE_H__
