#include <foundation/include/base/SHA1.h>
#include <foundation/include/crypt/Crypt.h>

namespace ws {

namespace SHA1 {

ustring encrypt(const ustring& in) {
	return Crypt::sha1(in);
}

}

}