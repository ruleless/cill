#ifndef __KLIBDEF_H__
#define __KLIBDEF_H__

#ifndef container_of
#define container_of(ptr, type, member) \
    (type *)((char *)(ptr) - (char *)&((type *)0)->member)
#endif

#endif // __KLIBDEF_H__
