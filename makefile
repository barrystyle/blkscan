CC=g++
CPPFLAGS=-O2 -I. -I./src -I./src/secp256k1/include -std=c++17
LDFLAGS=-O2 -lm -lstdc++ -lboost_filesystem -lsecp256k1

SOURCES= \
        src/arith_uint256.cpp \
        src/base58.cpp \
        src/fs.cpp \
        src/hash.cpp \
        src/main.cpp \
        src/random.cpp \
        src/randomenv.cpp \
        src/uint256.cpp \
        src/crypto/aes.cpp \
        src/crypto/chacha20.cpp \
        src/crypto/chacha_poly_aead.cpp \
        src/crypto/hkdf_sha256_32.cpp \
        src/crypto/hmac_sha256.cpp \
        src/crypto/hmac_sha512.cpp \
        src/crypto/muhash.cpp \
        src/crypto/poly1305.cpp \
        src/crypto/ripemd160.cpp \
        src/crypto/sha1.cpp \
        src/crypto/sha256_avx2.cpp \
        src/crypto/sha256.cpp \
        src/crypto/sha256_shani.cpp \
        src/crypto/sha256_sse41.cpp \
        src/crypto/sha256_sse4.cpp \
        src/crypto/sha3.cpp \
        src/crypto/sha512.cpp \
        src/crypto/siphash.cpp \
        src/script/script.cpp \
        src/script/standard.cpp \
        src/primitives/block.cpp \
        src/primitives/transaction.cpp \
        src/support/cleanse.cpp \
        src/support/lockedpool.cpp \
        src/util/strencodings.cpp \
        src/util/string.cpp \
        src/util/time.cpp \
        src/bech32.cpp \
        src/pubkey.cpp \
        src/key.cpp \
        src/key_io.cpp \
        src/outputtype.cpp \
        src/indexman.cpp \
        src/blockparse.cpp

OBJECTS=$(SOURCES:%.cpp=%.o)
OUTPUT=blkscan

all: $(SOURCES) $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDLIBS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f blkscan
	rm -f src/*.o
	rm -f src/crypto/*.o
	rm -f src/script/*.o
	rm -f src/support/*.o
	rm -f src/util/*.o
	rm -f src/primitives/*.o
