#include "Ini.h"
#include "Trace.h"

NAMESPACE_BEG(core)

Ini::~Ini()
{
	save();
}

void Ini::_parse()
{
	clear();
	FILE *pFile = fopen(mPath, "r");
	if (NULL == pFile)
	{
		return;
	}

	char line[MAX_BUF] = {0};
	while (fgets(line, MAX_BUF, pFile) != NULL)
	{
		parse(line, strlen(line));
	}	
	fclose(pFile);

	if (mCurState != mStates[IniState_Start] && mCurState != mStates[IniState_Error])
	{
		mCurState->parse('\n');
	}
}

void Ini::onParseError()
{
	logErrorLn("parse "<<mPath<<" error!");
}

int Ini::getInt(const char* section, const char* key, int def) const
{
	std::string retStr = getString(section, key);
	if (retStr == "")
		return def;
	else
		return atoi(retStr.c_str());	
}

bool Ini::setInt(const char* section, const char* key, int val)
{
	char strVal[11] = {0};
	__snprintf(strVal, sizeof(strVal), "%d", val);
	setString(section, key, strVal);
	setDirty(true);
	return true;
}

void Ini::save()
{
	if (!mbDirty)
		return;

	FILE *pFile = fopen(mPath, "w+");
	if (pFile)
	{
		Sections::const_iterator itSec = mSecs.begin();
		for (; itSec != mSecs.end(); ++itSec)
		{
			if ((*itSec).secName != "")
			{
				fprintf(pFile, "[%s]\n", (*itSec).secName.c_str());
			}

			const Records &r = (*itSec).r;
			if (!r.empty())
			{
				Records::const_iterator it = r.begin();
				for (; it != r.end(); ++it)
				{
					if ((it->second).comment != "")
						fprintf(pFile, "%s = %s #%s\n", (it->first).c_str(),
								(it->second).val.c_str(), (it->second).comment.c_str());
					else
						fprintf(pFile, "%s = %s\n", (it->first).c_str(), (it->second).val.c_str());
				}
			}
			fputc('\n', pFile);
		}
		
		setDirty(false);
		fclose(pFile);
	}
}

NAMESPACE_END // namespace core
