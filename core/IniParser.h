#ifndef __INIPARSER_H__
#define __INIPARSER_H__

#include "CoreBase.h"

NAMESPACE_BEG(core)

enum EIniState
{
	IniState_Start = 0,
	IniState_Error,

	IniState_Comment,
	IniState_Sec,
	IniState_KeyVal,

	IniState_Max,
};

class IniParser;

//--------------------------------------------------------------------------
class IniState
{
  public:	   
	IniState(IniParser *parser)
			:mParser(parser)
	{}
	
    virtual ~IniState() {}

	virtual void parse(char c) {}
	virtual void onEnter(char c) {}
	virtual void onLeave(char c) {}
  protected:
	IniParser *mParser;
};

class IniStateStart : public IniState
{
  public:
    IniStateStart(IniParser *parser)
			:IniState(parser)
	{}
	
    virtual ~IniStateStart() {}
	
	virtual void parse(char c);
};

class IniStateError : public IniState
{
  public:
    IniStateError(IniParser *parser)
			:IniState(parser)
	{}
	
    virtual ~IniStateError() {}
	
	virtual void onEnter(char c);
};

class IniStateComment : public IniState
{
  public:
    IniStateComment(IniParser *parser)
			:IniState(parser)
			,mLength(0)
	{
		memset(mComment, 0, sizeof(mComment));
	}
	
    virtual ~IniStateComment() {}

	virtual void parse(char c);
	virtual void onEnter(char c);
	virtual void onLeave(char c);
  private:
	char mComment[MAX_BUF];
	int mLength;
};

class IniStateSec : public IniState
{
  public:
    IniStateSec(IniParser *parser)
			:IniState(parser)
			,mLength(0)
	{
		memset(mSec, 0, sizeof(mSec));
	}
	
    virtual ~IniStateSec() {}

	virtual void parse(char c);
	virtual void onEnter(char c);
	virtual void onLeave(char c);
  private:
	char mSec[MAX_BUF];
	int mLength;	
};

class IniStateKeyVal : public IniState
{
  public:
    IniStateKeyVal(IniParser *parser)
			:IniState(parser)
			,mKeyLength(0)
			,mValueLength(0)
			,mReadState(KeyValState_ReadKey)
	{
		memset(mKey, 0, sizeof(mKey));
		memset(mValue, 0, sizeof(mValue));
	}
	
    virtual ~IniStateKeyVal() {}

	virtual void parse(char c);
	virtual void onEnter(char c);
	virtual void onLeave(char c);
  private:
	enum EKeyValState
	{
		KeyValState_ReadKey,
		KeyValState_PreSpace,
		KeyValState_PostSpace,
		KeyValState_ReadVal,
	};
	
	char mKey[MAX_BUF];
	char mValue[MAX_BUF];
	int mKeyLength;
	int mValueLength;
	EKeyValState mReadState;
};
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
class IniParser
{
  public:
	struct SValue
	{
		std::string val, comment;

		SValue()
				:val(""), comment("")
		{}

		explicit SValue(const std::string &v)
				:val(v), comment("")
		{}
		
		SValue(const std::string &v, const std::string &c)
				:val(v), comment(c)
		{}
	};	
	typedef std::map<std::string, SValue> Records;
	
	struct Section
	{
		std::string secName;
		Records r;

		Section()
				:secName(""), r()
		{}
		
		explicit Section(const std::string &sec)
				:secName(sec)
				,r()
		{
		}		
	};
	typedef std::vector<Section> Sections;
	
    IniParser();	
    virtual ~IniParser();

	void clear();
	void setCurSecStr(const std::string &sec);
	void addRecord(const std::string &key, const std::string &val);
	void addComment(const std::string &comment);
	
	void parse(const char *str, int len);
	void gotoState(EIniState stat, char c);

	virtual void onParseError() = 0;
	
	std::string getString(const char* section, const char* key, const char* def = "") const;
	void setString(const char* section, const char* key, const char* val);

  protected:
	IniState* mCurState;
	IniState* mStates[IniState_Max];

	Sections mSecs;
	std::string mLastKey;
};
//--------------------------------------------------------------------------

NAMESPACE_END // namespace core

#endif // __INIPARSER_H__
