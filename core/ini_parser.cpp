#include "ini_parser.h"

NAMESPACE_BEG(core)

static char _toSymbol(char c)
{
    char s = 'e';
    switch (c)
    {
    case ' ':
    case '\r':
        s = 's';            
        break;
    case '[':
    case ']':
    case '=':
    case '\n':
    case '#':
        s = c;
        break;
    default:        
        if (c >= '0' && c <= '9')
        {
            s = 'w';
        }
        else if (c >= 'a' && c <= 'z')
        {
            s = 'w';
        }
        else if (c >= 'A' && c <= 'Z')
        {
            s = 'w';
        }
        else
        {
            static char s_charSet[] = {':', '.'};
            for (int i = 0; i < sizeof(s_charSet)/sizeof(char); ++i)
            {
                if (c == s_charSet[i])
                {
                    s = 'w';
                    break;
                }
            }
        }
        break;
    }
    return s;
}

//--------------------------------------------------------------------------
void IniStateStart::parse(char c)
{
    char s = _toSymbol(c);
    switch (s)
    {   
    case '[':
        mParser->gotoState(IniState_Sec, c);
        break;
    case 'w':
        mParser->gotoState(IniState_KeyVal, c);
        break;
    case '#':
        mParser->gotoState(IniState_Comment, c);
        break;
    case 's':
    case '\n':
        break;
    default:
        mParser->gotoState(IniState_Error, c);
        break;
    }
}

void IniStateError::onEnter(char c)
{
    mParser->onParseError();
}

void IniStateComment::parse(char c)
{
    if (c == '\n')
    {
        mParser->gotoState(IniState_Start, c);
        return;
    }
    mComment[mLength++] = c;
}

void IniStateComment::onEnter(char c)
{
    mComment[mLength=0] = '\0';
}

void IniStateComment::onLeave(char c)
{
    if (mComment[0] != '\0')
        mParser->addComment(mComment);
}

void IniStateSec::parse(char c)
{
    char s = _toSymbol(c);
    if (s == 'w')
    {
        mSec[mLength++] = c;
        mSec[mLength] = '\0';
    }
    else if (s == ']')
    {
        mParser->gotoState(IniState_Start, c);
    }
}

void IniStateSec::onEnter(char c)
{
    mSec[mLength=0] = '\0';
}

void IniStateSec::onLeave(char c)
{
    mParser->setCurSecStr(mSec);
}

void IniStateKeyVal::parse(char c)
{
    char s = _toSymbol(c);
    switch (mReadState)
    {
    case KeyValState_ReadKey:
        if (s == 'w')
        {
            mKey[mKeyLength++] = c;
            mKey[mKeyLength] = '\0';
        }
        else if (s == 's')
        {
            mReadState = KeyValState_PreSpace;
        }
        else if (s == '=')
        {
            mReadState = KeyValState_PostSpace;
        }
        else
        {
            mParser->gotoState(IniState_Error, c);
        }
        
        break;
    case KeyValState_PreSpace:
        if (s == '=')
        {
            mReadState = KeyValState_PostSpace;
        }
        else if (s != 's')
        {
            mParser->gotoState(IniState_Error, c);
        }
        
        break;
    case KeyValState_PostSpace:
        if (s == 'w')
        {
            mReadState = KeyValState_ReadVal;
            mValue[mValueLength++] = c;
            mValue[mValueLength] = '\0';
        }
        else if (s != 's')
        {
            mParser->gotoState(IniState_Error, c);
        }
        
        break;
    case KeyValState_ReadVal:
        if (s == 'w')
        {
            mValue[mValueLength++] = c;
            mValue[mValueLength] = '\0';
        }
        else if (s == 's' || s == '\n')
        {
            mParser->gotoState(IniState_Start, c);
        }
        else
        {
            mParser->gotoState(IniState_Error, c);
        }
        
        break;
    }
}

void IniStateKeyVal::onEnter(char c)
{
    mKey[mKeyLength=0] = c;
    mKey[++mKeyLength] = '\0';
    mValue[mValueLength=0] = '\0';
    mReadState = KeyValState_ReadKey;
}

void IniStateKeyVal::onLeave(char c)
{
    mParser->addRecord(mKey, mValue);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
IniParser::IniParser()
        :mLastKey("")
{
    mStates[IniState_Start] = new IniStateStart(this);
    mStates[IniState_Error] = new IniStateError(this);
    mStates[IniState_Comment] = new IniStateComment(this);
    mStates[IniState_Sec] = new IniStateSec(this);
    mStates[IniState_KeyVal] = new IniStateKeyVal(this);

    mCurState = mStates[IniState_Start];
}

IniParser::~IniParser()
{
    for (int i = 0; i < IniState_Max; ++i)
    {
        delete mStates[i];
    }   
}

void IniParser::clear()
{
    Sections::iterator it = mSecs.begin();
    for (; it != mSecs.end(); ++it)
    {
        (*it).r.clear();
    }
    mSecs.clear();
    mLastKey.clear();
}

void IniParser::setCurSecStr(const std::string &sec)
{
    if (mSecs.size() > 0 && mSecs.back().secName == sec)
    {
        return;
    }
    
    mSecs.push_back(Section(sec));
}

void IniParser::addRecord(const std::string &key, const std::string &val)
{
    if (mSecs.size() == 0)
    {
        mSecs.push_back(Section(""));
    }
    
    Section &s = mSecs.back();
    s.r[key] = SValue(val);
    mLastKey = key;
}

void IniParser::addComment(const std::string &comment)
{
    if (!mSecs.empty() && mLastKey != "")
    {
        Section &s = mSecs.back();
        Records::iterator it = s.r.find(mLastKey);
        if (it != s.r.end())
        {
            it->second.comment = comment;
        }
    }   
}
    
void IniParser::parse(const char *str, int len)
{
    for (int i = 0; i < len; ++i)
    {
        mCurState->parse(str[i]);
    }
}

void IniParser::gotoState(EIniState stat, char c)
{
    mCurState->onLeave(c);
    mCurState = mStates[stat];
    mCurState->onEnter(c);
}

std::string IniParser::getString(const char* section, const char* key, const char* def) const
{
    Sections::const_iterator it = mSecs.begin();
    for (; it != mSecs.end(); ++it)
    {
        const Section &s = *it;
        if (s.secName == section)
        {
            Records::const_iterator itRet = s.r.find(key);
            if (itRet != s.r.end())
            {
                return itRet->second.val;
            }
        }
    }
    return "";
}

void IniParser::setString(const char* section, const char* key, const char* val)
{
    Sections::iterator it = mSecs.begin();
    for (; it != mSecs.end(); ++it)
    {
        Section &s = *it;
        if (s.secName == section)
        {
            s.r[key].val = val;
            return;
        }
    }

    Section s(section);
    s.r[key].val = val;
    mSecs.push_back(s);
}
//--------------------------------------------------------------------------

NAMESPACE_END // namespace core
