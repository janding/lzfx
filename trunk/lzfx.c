/*
 * Copyright (c) 2009 Andrew Collette <andrew.collette at gmail.com>
 *
 * Implements an LZF-compatible compressor/decompressor based on the liblzf
 * codebase written by Marc Lehmann.  This code is released under the BSD
 * license.  License and original copyright statement follow.
 *
 * 
 * Copyright (c) 2000-2008 Marc Alexander Lehmann <schmorp@schmorp.de>
 * 
 * Redistribution and use in source and binary forms, with or without modifica-
 * tion, are permitted provided that the following conditions are met:
 * 
 *   1.  Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 * 
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MER-
 * CHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTH-
 * ERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lzfx.h"

#define LZFX_HSIZE (1 << (LZFX_HLOG))

/* We need this for memset */
#ifdef __cplusplus
# include <cstring>
#else
# include <string.h>
#endif

#if __GNUC__ >= 3
# define fx_expect_false(expr)  __builtin_expect((expr) != 0, 0)
# define fx_expect_true(expr)   __builtin_expect((expr) != 0, 1)
#else
# define fx_expect_false(expr)  (expr)
# define fx_expect_true(expr)   (expr)
#endif

typedef unsigned char u8;
typedef const u8 *LZSTATE[LZFX_HSIZE];

/* Define the hash function */
#define LZFX_FRST(p)     (((p[0]) << 8) | p[1])
#define LZFX_NEXT(v,p)   (((v) << 8) | p[2])
#define LZFX_IDX(h)      ((( h >> (3*8 - LZFX_HLOG)) - h  ) & (LZFX_HSIZE - 1))

/* These cannot be changed, as they are related to the compressed format. */
#define LZFX_MAX_LIT        (1 <<  5)
#define LZFX_MAX_OFF        (1 << 13)
#define LZFX_MAX_REF        ((1 << 8) + (1 << 3))


/*
 * compressed format
 *
 * 000LLLLL <L+1>    ; literal
 * LLLooooo oooooooo ; backref L
 * 111ooooo LLLLLLLL oooooooo ; backref L+7
 *
 */

int lzfx_compress(const void* ibuf, unsigned int ilen,
                        void* obuf, unsigned int *olen){

    LZSTATE htab;

    const u8 **hslot;
    const u8 *ip = (const u8 *)ibuf;
    const u8 *in_end = ip + ilen;
    u8 *op = (u8 *)obuf;
    u8 *out_end = NULL;

    const u8 *ref;
    unsigned int hval;
    int lit;
#if defined (WIN32) && defined (_M_X64)
    unsigned _int64 off; /* workaround for missing POSIX compliance */
#else
    unsigned long off;
#endif

    if(ibuf == NULL || obuf == NULL || olen == NULL) return LZFX_EARGS;
    
    if(ilen==0){
        *olen = 0;
        return 0;
    }

    out_end = op + *olen;

    memset(htab, 0, sizeof(htab));

    lit = 0; op++; /* Literal counter: number of bytes in the current run */

    hval = LZFX_FRST(ip);

    while(ip < in_end -2){
        hval = LZFX_NEXT(hval, ip);
        hslot = htab + LZFX_IDX(hval);

        ref = *hslot;   /* Location of possible match at ref */
        *hslot = ip;

        if( ref < ip
        &&  (off = ip - ref - 1) < LZFX_MAX_OFF
        &&  ip + 4 < in_end
        &&  ref > (u8*)ibuf
        &&  ref[0] == ip[0]
        &&  ref[1] == ip[1]
        &&  ref[2] == ip[2] ) {

            unsigned int len = 2;
            unsigned int maxlen = in_end - ip -len;
            maxlen = maxlen > LZFX_MAX_REF ? LZFX_MAX_REF : maxlen;

            if(fx_expect_false(op - !lit + 3 + 1 >= out_end))
                return LZFX_ESIZE;

            /*  Reach back to the beginning of the current literal run and
                set its final size.  Note this is why we skipped the first
                byte of the output; we set it here. */
            op [- lit - 1] = lit - 1;
            op -= !lit;               /* Undo run if length is zero */

            /*  Determine the number of matching bytes, up to maxlen. */
            do
                len++;
            while (len < maxlen && ref[len] == ip[len]);


            len -= 2; /* len is now #octets - 1 */
            ip++;

            /* Format 1: [LLLooooo oooooooo] */
            if (len < 7) {
              *op++ = (off >> 8) + (len << 5);

            /* Format 2: [111ooooo LLLLLLLL oooooooo] */
            } else {
              *op++ = (off >> 8) + (  7 << 5);
              *op++ = len - 7;
            }

            *op++ = off;

            lit = 0; op++; /* start run */

            ip += len + 1;

            if (fx_expect_false (ip >= in_end - 2)) break;

            --ip;

            hval = LZFX_FRST (ip);

            hval = LZFX_NEXT (hval, ip);
            htab[LZFX_IDX (hval)] = ip;
            ip++;

        } else {
              /* Keep copying literal bytes */

              if (fx_expect_false (op >= out_end)) return LZFX_ESIZE;

              lit++; *op++ = *ip++;

              if (fx_expect_false (lit == LZFX_MAX_LIT)) {
                  op [- lit - 1] = lit - 1; /* stop run */
                  lit = 0; op++; /* start run */
              }

        } /* if() found match in htab */

    } /* while(ip < ilen -2) */

    /*  We finish off the last few bytes with a literal run */

    /* at most 3 bytes can be missing here */
    if (op + 3 > out_end) return LZFX_ESIZE;

    while (ip < in_end)
    {
      lit++; *op++ = *ip++;

      if (fx_expect_false (lit == LZFX_MAX_LIT))
        {
          op [- lit - 1] = lit - 1; /* stop run */
          lit = 0; op++; /* start run */
        }
    }

    op [- lit - 1] = lit - 1; /* end run */
    op -= !lit; /* undo run if length is zero */

    *olen = op - (u8 *)obuf;
    return 0;
}

/* Decompressor */
int lzfx_decompress(const void* ibuf, unsigned int ilen,
                          void* obuf, unsigned int *olen){

    u8 const *ip = (const u8 *)ibuf;
    u8 const *const in_end = ip + ilen;
    u8 *op = (u8 *)obuf;
    u8 *out_end = NULL;

    if(ibuf == NULL || obuf == NULL || olen == NULL) return LZFX_EARGS;
    
    if(ilen == 0){
        *olen = 0;
        return 0;
    }

    out_end = op + *olen;

    do {
        unsigned int ctrl = *ip++;

        /* Format 000LLLLL: a literal byte string follows, of length L+1 */
        if(ctrl < (1 << 5)) {

            ctrl++;

            if(op + ctrl > out_end) return LZFX_ESIZE;
            if(ip + ctrl > in_end) return LZFX_ECORRUPT;

            do
                *op++ = *ip++;
            while(--ctrl);

        /*  Format #1 [LLLooooo oooooooo]: backref of length L (+2)
                          ^^^^^ ^^^^^^^^
                            A      B
                   #2 [111ooooo LLLLLLLL oooooooo] backref of length L+7 (+2)
                          ^^^^^          ^^^^^^^^
                            A               B
            In both cases the location of the backref is computed from the
            remaining part of the data as follows:

                location = op - A*256 - B - 1
        */
        } else {

            unsigned int len = (ctrl >> 5);
            u8 *ref = op - ((ctrl & 0x1f) << 8) -1;

            if(len==7){     /* i.e. format #2 */
                len += *ip++;
            }

            if(ip >= in_end) return LZFX_ECORRUPT;

            ref -= *ip++;

            if(op + len + 2 > out_end) return LZFX_ESIZE;
            if(ref < (u8*)obuf) return LZFX_ECORRUPT;

            *op++ = *ref++;
            *op++ = *ref++;

            do
                *op++ = *ref++;
            while (--len);
        }

    } while (ip < in_end);

    *olen = op - (u8 *)obuf;

    return 0;
}



