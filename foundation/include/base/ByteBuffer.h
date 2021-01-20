#ifndef _H_LIBCOMMON_BYTEBUFFER_2020_10_28_17_31_17
#define _H_LIBCOMMON_BYTEBUFFER_2020_10_28_17_31_17

#include <helper/Types.h>
#include <helper/ByteOrder.h>
#include <helper/Uncopyable.h>

namespace ws {

class ByteBuffer {
	kDisableCopyAssign(ByteBuffer);

public:
	ByteBuffer(u32 bytes = 1024);
	~ByteBuffer();

public:	
	i8* begin();
	void clear();
	u32 size() const;
	u32 readedBytes();
	u32 writedBytes();
	void moveReadPos(u32 pos);
	void moveWritePos(u32 pos);
	u32 readableBytes() const;
	u32 writeableBytes() const;

public:
	u16 readU16();
	u32 readU32();
	u64 readU64();

	u16 peekU16(u32 pos);
	u32 peekU32(u32 pos);
	u64 peekU64(u32 pos);

	void writeU16(u16 val);
	void writeU32(u32 val);
	void writeU64(u64 val);

	/*******************************************************************
	*@brief  : 读取失败将返回空指针,
	********************************************************************/
	const i8* read(u32 bytes);
	void ensureWriteable(u32 bytes);
	void write(const i8* val, u32 bytes);

private:
	void shift();
	const i8* peek(u32 peekBytes, i32 pos) const;

private:
	i8* _buffer;
	u32 _readed;
	u32 _writed;
	u32 _capacity;
	u32 _defaultCapacity;
};

}

#endif //_H_LIBCOMMON_BYTEBUFFER_2020_10_28_17_31_17