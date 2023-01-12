Downloads, bug tracker, documentation are at Google Code:
http://lzfx.googlecode.com

Contact email: andrew dot collette at gmail dot com

## What is LZFX? ##

LZFX is a tiny, extremely fast compression library compatible with liblzf.
It was originally developed in support of the HDF5 for Python project
(h5py.googlecode.com).

The goal of LZFX is to provide high-speed compression of redundant data.
Compression ratios are therefore not as good as other compression utilities
like gzip or bzip.  Until fast RLE encoders, LZFX uses dictionary-style
compression allowing it to deal with arbitrary repeated substrings, not just
runs of identical characters.

## How do I use it? ##

The library consists of two files, lzfx.c and lzfx.h.  Please read lzfx.h for
API information.  Like LZF, you simply supply an input and output buffer.
There are no compression settings to adjust.  You can link against liblzfx.a
or simply copy the two source files into your project.  Since LZFX is BSD
licensed, these approaches are legal for both open-source and proprietary
applications.

## Is there a compression utility? ##

Yes, an early version of a compression program (named "lzfx") is automatically
built along with the library.  Simply run

`  $ lzfx <infile> <outfile> <c|d> `

to compress or decompress data.  The file format is not expected to change, but
until LZFX reaches 1.0 status you are STRONGLY ADVISED not to use this utility
on critical data.

## Acknowledgements ##

LZFX is based on the LZF code base by Marc Lehmann.  Changes include a new
API to make it easier to use with HDF5/h5py, code refactoring, and changes
to the decompressor to make it easier to determine the required size of a
decompression buffer.  The LZFX compressor and decompressor are 100% compatible
with existing LZF streams, although the file format used by the "lzfx" demo
utility is different than that used by the "lzf" utility.

## Known limitations ##

  * The lzfx utility is not full-featured yet
  * You will need LZF installed to run "make check" as part of the test suite checks for cross-compatibility
  * A benchmark program is not yet available

