#include <foundation\include\base\ByteBuffer.h>

namespace ws {

ByteBuffer::ByteBuffer(u32 bytes) :
	_readed(0),
	_writed(0),
	_capacity(bytes),
	_buffer(nullptr),
	_defaultCapacity(bytes) {
	_buffer = (i8*)::malloc(_defaultCapacity);
}

ByteBuffer::~ByteBuffer() {
	if(nullptr != _buffer) {
		::free(_buffer);
		_buffer = nullptr;
	}
}

void ByteBuffer::clear() {
	shift();
	_writed = 0;
	_readed = 0;
}

i8* ByteBuffer::begin() { 
	return _buffer; 
}

u32 ByteBuffer::size() const {
	return _capacity; 
}

u32 ByteBuffer::readedBytes() { 
	return _readed; 
}

u32 ByteBuffer::writedBytes() {
	return _writed; 
}

void ByteBuffer::moveReadPos(u32 pos) { 
	assert(pos <= _writed);
	_readed = (std::min)(pos, _writed); 
}

void ByteBuffer::moveWritePos(u32 pos) { 
	assert(pos <= _capacity);
	_writed = (std::min)(pos, _capacity);
}

u32 ByteBuffer::readableBytes() const {
	return _writed - (std::min)(_readed, _writed);
}

u32 ByteBuffer::writeableBytes() const {
	return _capacity - (std::min)(_writed, _capacity);
}

u16 ByteBuffer::readU16() {
	auto v = reinterpret_cast<const u16*>(read(sizeof(u16)));
	return (nullptr != v) ? btoh16(*v) : 0;
}

u32 ByteBuffer::readU32() {
	auto v = reinterpret_cast<const u32*>(read(sizeof(u32)));
	return (nullptr != v) ? btoh32(*v) : 0;
}

u64 ByteBuffer::readU64() {
	auto v = reinterpret_cast<const u64*>(read(sizeof(u64)));
	return (nullptr != v) ? btoh64(*v) : 0;
}

u16 ByteBuffer::peekU16(u32 pos) {
	auto v = reinterpret_cast<const u16*>(peek(sizeof(u16), pos));
	return (nullptr != v) ? btoh16(*v) : 0;
}

u32 ByteBuffer::peekU32(u32 pos) {
	auto v = reinterpret_cast<const u32*>(peek(sizeof(u32), pos));
	return (nullptr != v) ? btoh32(*v) : 0;
}

u64 ByteBuffer::peekU64(u32 pos) {
	auto v = reinterpret_cast<const u64*>(peek(sizeof(u64), pos));
	return (nullptr != v) ? btoh64(*v) : 0;
}

const i8* ByteBuffer::read(u32 bytes) {
	assert(readableBytes() >= bytes);
	return (readableBytes() >= bytes) ? 
		(_readed += bytes, _buffer + (_readed - bytes)) : nullptr;
}

void ByteBuffer::writeU16(u16 val) {
	val = htob16(val);
	write(reinterpret_cast<const i8*>(&val), sizeof(val));
}

void ByteBuffer::writeU32(u32 val) {
	val = htob32(val);
	write(reinterpret_cast<const i8*>(&val), sizeof(val));
}

void ByteBuffer::writeU64(u64 val) {
	val = htob64(val);
	write(reinterpret_cast<const i8*>(&val), sizeof(val));
}

void ByteBuffer::ensureWriteable(u32 bytes) {
	if(writeableBytes() < bytes) {
		_capacity = _capacity + bytes + 1024;
		_buffer = (i8*)::realloc(_buffer, _capacity);
	}
}

void ByteBuffer::write(const i8* val, u32 bytes) {
	ensureWriteable(bytes);
	memcpy(_buffer + _writed, val, bytes);
	_writed += bytes;
}

void ByteBuffer::shift() {
	if(_capacity > _defaultCapacity) {
		_capacity = _defaultCapacity;
		_buffer = (i8*)::realloc(_buffer, _defaultCapacity);
	}
}

const i8* ByteBuffer::peek(u32 peekBytes, i32 pos) const {
	assert(_writed >= (pos + peekBytes));
	return (_writed >= (pos + peekBytes)) ? (_buffer + pos) : nullptr;
}

}