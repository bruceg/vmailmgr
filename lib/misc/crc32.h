/*********************************** CRC32 ***********************************/

/* Use a type crc32_t variable to store the crc value.
 * Initialise the variable to CRC32INIT before running the crc routine.
 * for more details see the CRC32.C file */

#ifndef CRC32INIT
#define CRC32INIT 0xFFFFFFFFL /* the initializer for the 32-bit CRC */

typedef unsigned long crc32_t; /* the type of the 32-bit CRC */

extern const crc32_t crc32tab[]; /* a table of 32-bit CRC feedback terms */

#ifdef __cplusplus

inline crc32_t crc32_update (const crc32_t crc, const unsigned char octet)
{
  return crc32tab [(unsigned char) crc ^ octet] ^ ((crc >> 8) & 0x00FFFFFFL);
}

class crc32_c
{
private:
  crc32_t val;
public:
  inline crc32_c (const crc32_t v = 0L) : val (v ^ CRC32INIT) {}
  inline void init () { val = CRC32INIT; }
  inline operator crc32_t () const { return val ^ CRC32INIT; }
  inline crc32_t operator = (const crc32_c &n) {
    return (val = n.val) ^ CRC32INIT;
  }
  inline crc32_t operator = (const crc32_t &n) {
    return val = n ^ CRC32INIT, n;
  }
  inline void update (const unsigned char octet) {
    val = crc32_update (val, octet);
  }
  crc32_t update (const unsigned char *block, int len);
  inline crc32_t update (const char* block, int len) {
    return update((const unsigned char*)block, len);
  }
};
#else
#define crc32_update(crc32,octet) ( crc32 = crc32tab[(unsigned char)(crc32) ^ \
        (unsigned char)(octet) ] ^ ( ((crc32)>>8) & 0x00FFFFFFL ) )
#endif

#endif
