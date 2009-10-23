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

#ifndef LZFX_H
#define LZFX_H

#ifdef __cplusplus
extern "C" {
#endif

/*  Documented behavior, including function signatures and error codes,
    is guaranteed to remain unchanged for releases with the same major
    version number.  Releases of the same major version are also able
    to read each other's output, although the output itself is not
    guaranteed to be exactly the same.
*/
#define LZFX_VERSION_MAJOR      0
#define LZFX_VERSION_MINOR      1
#define LZFX_VERSION_STRING     "0.1"

/* Hashtable size (2**LZFX_HLOG entries) */
#ifndef LZFX_HLOG
# define LZFX_HLOG 16
#endif

/* Predefined errors. */
#define LZFX_ESIZE      -1      /* Output buffer too small */
#define LZFX_ECORRUPT   -2      /* Invalid data for decompression */
#define LZFX_EARGS      -3      /* Arguments invalid (NULL) */

/*  Buffer-to-buffer compression routines.  Provide an input and output
    buffer, along with their sizes in bytes.  Upon successful compression or
    decompression, olen contains the number of bytes written, and the function
    returns a non-negative value.  If an error occurs, olen is not modified,
    and a negative value is returned.  The contents of the output buffer after
    an error are undefined.

    Overlapping buffers are not permitted; the results are undefined.  NULL
    pointer arguments result in an LZFX_EARGS error code.  A zero value for
    ilen results in short-circuit success; a zero value for olen results in
    the error code LZFX_ESIZE.
*/    
int lzfx_compress(const void* ibuf, unsigned int ilen,
                        void* obuf, unsigned int *olen);

int lzfx_decompress(const void* ibuf, unsigned int ilen,
                          void* obuf, unsigned int *olen);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
