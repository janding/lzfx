# This project is now unmaintained #

LZFX began life as a reboot of the LZF compression library, for use with the h5py Python HDF5 bindings.  The original author has since moved on.  If you are interested in taking over maintenance of LZFX, please contact me at (andrew dot collette at gmail dot com).

## About ##

LZFX is a small (one C file, 200 non-comment lines) BSD-licensed library designed for very-high-speed compression of redundant data.  It is descended from [liblzf](http://home.schmorp.de/marc/liblzf.html) and is 100% compatible with existing LZF-compressed data.

LZFX was originally developed as a component of the [h5py project](http://h5py.googlecode.com), which uses an LZF-based compressor to process scientific data.

## Details ##

The API is extremely simple.  There are no knobs to adjust, except the (compile-time) option of changing the hashtable size.  The following function prototypes are reproduced from lzfx.h:

```
/*  Buffer-to buffer compression.

    Supply pre-allocated input and output buffers via ibuf and obuf, and
    their size in bytes via ilen and olen.  Buffers may not overlap.

    On success, the function returns a non-negative value and the argument
    olen contains the compressed size in bytes.  On failure, a negative
    value is returned and olen is not modified.
*/
int lzfx_compress(const void* ibuf, unsigned int ilen,
                        void* obuf, unsigned int *olen);

/*  Buffer-to-buffer decompression.

    Supply pre-allocated input and output buffers via ibuf and obuf, and
    their size in bytes via ilen and olen.  Buffers may not overlap.

    On success, the function returns a non-negative value and the argument
    olen contains the uncompressed size in bytes.  On failure, a negative
    value is returned.

    If the failure code is LZFX_ESIZE, olen contains the minimum buffer size
    required to hold the decompressed data.  Otherwise, olen is not modified.

    Supplying a zero *olen is a valid and supported strategy to determine the
    required buffer size.  This does not require decompression of the entire
    stream and is consequently very fast.  Argument obuf may be NULL in
    this case only.
*/
int lzfx_decompress(const void* ibuf, unsigned int ilen,
                          void* obuf, unsigned int *olen);
```