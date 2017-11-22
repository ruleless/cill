#include "memorystream.h"

NAMESPACE_BEG(core)

static ObjectPool<MemoryStream> s_ObjPool("MemoryStream");

ObjectPool<MemoryStream>& MemoryStream::ObjPool()
{
    return s_ObjPool;
}

void MemoryStream::destroyObjPool()
{
    ErrorPrint("MemoryStream::destroyObjPool(): size %d", (int)s_ObjPool.size());

    s_ObjPool.destroy();
}

size_t MemoryStream::getPoolObjectBytes()
{
    size_t bytes = sizeof(rpos_) + sizeof(wpos_) + data_.capacity();
    return bytes;
}

void MemoryStream::onReclaimObject()
{
    if(data_.capacity() > DEFAULT_SIZE * 2)
        data_.reserve(DEFAULT_SIZE);

    clear(false);
}

NAMESPACE_END // namespace core
