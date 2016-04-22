#ifndef __CMD_INPUT_H__
#define __CMD_INPUT_H__

#include "CmdBase.h"

NAMESPACE_BEG(cmd)

class Input : public TPTask
{
  public:
	struct Listener 
	{
		virtual void onInputDestroy(Input *pInput) = 0;
	};
	
	Input(Listener *pListener);
	virtual ~Input();

	inline const std::string& getCmdLine()
	{
		return mNextCmdLine;
	}
	inline void clearCmdLine()
	{
		mNextCmdLine.clear();
	}

	virtual bool process();
	virtual TPTask::TPTaskState presentMainThread();
  protected:
	std::string mInputBuffer;
	std::string mNextCmdLine;
	Listener *mpListener;
};

NAMESPACE_END // namespace cmd

#endif // __CMD_INPUT_H__
