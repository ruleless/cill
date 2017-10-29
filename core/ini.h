#ifndef __CORE_INI_H__
#define __CORE_INI_H__

#include "corebase.h"
#include "ini_parser.h"

NAMESPACE_BEG(core)

class Ini : public IniParser
{
  public:
    Ini(const char *pathname)
            :IniParser()
            ,mbDirty(false)
    {
        memset(mPath, 0, sizeof(mPath));
        strncpy(mPath, pathname, sizeof(mPath)-1);
        _parse();
    }
    
    virtual ~Ini();

    void _parse();

    virtual void onParseError();

    int getInt(const char* section, const char* key, int def = 0) const;
    bool setInt(const char* section, const char* key, int val);     

    void save();

    inline void setDirty(bool bDirty = true)
    {
        mbDirty = bDirty;
    }   
  private:  
    char mPath[MAX_PATH];
    bool mbDirty;
};

NAMESPACE_END // namespace core

#endif // __CORE_INI_H__
