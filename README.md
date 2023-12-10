# FileMagic

## DESCRIPTION

FileMagic is an extension that allows you to use **libmagic**(3) from ruby.

DO NOT USE!!! This is in the process of being improved.

## VERSION

This documentation refers to FileMagic version 0.9.0 and **libmagic**(3)
from **file**(1) version 5.45. If FileMagic is built against an earlier
version of **libmagic**(3), then some constants and methods described in this
document might not be available.

## Constants

### Flags

| Flag | Symbol | Description |
| --- | --- | --- |
| `MAGIC_NONE` | `:none` | No special handling. |
| `MAGIC_DEBUG` | `:debug` | Print debugging messages to stderr. |
| `MAGIC_SYMLINK` | `:symlink` | If the file queried is a symlink, follow it. |
| `MAGIC_COMPRESS` | `:compress` | If the file is compressed, unpack it and look at the contents. |
| `MAGIC_DEVICES` | `:devices` | If the file is a block or character special device, then open the device and try to look in its contents. |
| `MAGIC_MIME_TYPE` | `:mime_type` | Return a MIME type string, instead of a textual description. |
| `MAGIC_CONTINUE` | `:continue` | Return all matches, not just the first. |
| `MAGIC_CHECK` | `:check` | Check the magic database for consistency and print warnings to stderr. |
| `MAGIC_PRESERVE_ATIME` | `:preserve_atime` | On systems that support utime(3) or utimes(2), attempt to preserve the access time of files analysed. |
| `MAGIC_RAW` | `:raw` | Don't translate unprintable characters to a \ooo octal representation. |
| `MAGIC_ERROR` | `:error` | Treat operating system errors while trying to open files and follow symlinks as real errors, instead of printing them in the magic buffer. |
| `MAGIC_MIME_ENCODING` | `:mime_encoding` | Return a MIME encoding, instead of a textual description. |
| `MAGIC_MIME` | `:mime` | Shorthand for `MAGIC_MIME_TYPE \| MAGIC_MIME_ENCODING`. |
| `MAGIC_APPLE` | `:apple` | Return the Apple creator and type. |
| `MAGIC_EXTENSION` | `:extension` | Return a slash-separated list of extensions for this file type. |
| `MAGIC_COMPRESS_TRANSP` | `:compress_transp` | Don't report on compression, only report about the uncompressed data. |
| `MAGIC_NO_COMPRESS_FORK` | `:no_compress_fork` | Don't allow decompressors that use fork. |
| `MAGIC_NODESC` | `:nodesc` | Shorthand for `MAGIC_EXTENSION \| MAGIC_MIME \| MAGIC_APPLE`. |
| `MAGIC_NO_CHECK_COMPRESS` | `:no_check_compress` | Don't look inside compressed files. |
| `MAGIC_NO_CHECK_TAR` | `:no_check_tar` | Don't examine tar files. |
| `MAGIC_NO_CHECK_SOFT` | `:no_check_soft` | Don't consult magic files. |
| `MAGIC_NO_CHECK_APPTYPE` | `:no_check_apptype` | Don't check for EMX application type (only on EMX). |
| `MAGIC_NO_CHECK_ELF` | `:no_check_elf` | Don't print ELF details. |
| `MAGIC_NO_CHECK_TEXT` | `:no_check_text` | Don't check for various types of text files. |
| `MAGIC_NO_CHECK_CDF` | `:no_check_cdf` | Don't get extra information on MS Composite Document Files. |
| `MAGIC_NO_CHECK_CSV` | `:no_check_csv` | Don't examine CSV files. |
| `MAGIC_NO_CHECK_TOKENS` | `:no_check_tokens` | Don't look for known tokens inside ascii files. |
| `MAGIC_NO_CHECK_ENCODING` | `:no_check_encoding` | Don't check text encodings. |
| `MAGIC_NO_CHECK_JSON` | `:no_check_json` | Don't examine JSON files. |
| `MAGIC_NO_CHECK_SIMH` | `:no_check_simh` | Don't examine SIMH tape files. |
| `MAGIC_NO_CHECK_BUILTIN` | `:no_check_builtin` | No built-in tests; only consult the magic file. Shorthand for `MAGIC_NO_CHECK_COMPRESS \| MAGIC_NO_CHECK_TAR \| MAGIC_NO_CHECK_APPTYPE \| MAGIC_NO_CHECK_ELF \| MAGIC_NO_CHECK_TEXT \| MAGIC_NO_CHECK_CSV \| MAGIC_NO_CHECK_CDF \| MAGIC_NO_CHECK_TOKENS \| MAGIC_NO_CHECK_ENCODING \| MAGIC_NO_CHECK_JSON \| MAGIC_NO_CHECK_SIMH`. |
| `MAGIC_NO_CHECK_ASCII` | `:no_check_ascii` | Defined for backwards compatibility. Renamed to `MAGIC_NO_CHECK_TEXT`. |
| `MAGIC_NO_CHECK_FORTRAN` | `:no_check_fortran` | Don't check ascii/fortran. Defined for backwards compatibility; does nothing. |
| `MAGIC_NO_CHECK_TROFF` | `:no_check_troff` | Don't check ascii/troff. Defined for backwards compatibility; does nothing. |

### Parameters

| Parameter | Symbol | Description |
| --- | --- | --- |
| `MAGIC_PARAM_INDIR_MAX` | `:param_indir_max` | How many levels of recursion will be followed for indirect magic entries. Default is 50. |
| `MAGIC_PARAM_NAME_MAX` | `:param_name_max` | The maximum number of calls for name/use. Default is 50. |
| `MAGIC_PARAM_ELF_PHNUM_MAX` | `:param_elf_phnum_max` | How many ELF program sections will be processed. Default is 2,048. |
| `MAGIC_PARAM_ELF_SHNUM_MAX` | `:param_elf_shnum_max` | How many ELF sections will be processed. Default is 32,768. |
| `MAGIC_PARAM_ELF_NOTES_MAX` | `:param_elf_notes_max` | How many ELF notes will be processed. Default is 256. |
| `MAGIC_PARAM_REGEX_MAX` | `:param_regex_max` | Length limit for REGEX searches. Default is 8,192. |
| `MAGIC_PARAM_BYTES_MAX` | `:param_bytes_max` | Maximum number of bytes to look at inside a file. Default is 7,340,032. |
| `MAGIC_PARAM_ENCODING_MAX` | `:param_encoding_max` | Maximum number of bytes to scan for encoding. Default is 65,536. |
| `MAGIC_PARAM_ELF_SHSIZE_MAX` | `:param_elf_shsize_max` | Maximum ELF section size to process. Default is 134,217,728. |


## Methods

`file(filename)`
: Returns a textual description of the contents of the filename argument

`buffer(string)`
: Returns a textual description of the contents of the string argument

`check(filename)`
: Checks the validity of entries in the database file passed in as filename

`compile(filename)`
: Compiles the database file passed in as filename

`load(filename)`
: Loads the database file passed in as filename

`close()`
: Closes the magic database and frees any allocated memory


## Synopsis

```ruby
require 'filemagic'

p FileMagic::VERSION
# => "0.9.0"
p FileMagic::MAGIC_VERSION
# => "5.45"

p FileMagic.new.flags
# => []

FileMagic.open(:mime) { |fm|
  p fm.flags
  # => [:mime_type, :mime_encoding]
  p fm.file(__FILE__)
  # => "text/plain; charset=us-ascii"
  p fm.file(__FILE__, true)
  # => "text/plain"

  fm.flags = [:raw, :continue]
  p fm.flags
  # => [:continue, :raw]
}

fm = FileMagic.new
p fm.flags
# => []

mime = FileMagic.mime
p mime.flags
# => [:mime_type, :mime_encoding]
```

## Environment

The environment variable `MAGIC` can be used to set the default magic file
name. See **file**(1) man page for more information.

## Installation

Install the gem:

    sudo gem install ruby-filemagic

The **file**(1) library and headers are required. Commands/packages for some
operating systems are:

| System | Package |
| --- | --- |
| Alpine | `libmagic file file-dev` |
| Debian/Ubuntu | `apt install libmagic-dev` |
| Fedora/SuSE | `dnf install file-devel` |
| Gentoo | `sys-libs/libmagic` |
| OS X | `brew install libmagic` |


## Build native extension

    rake docker:gem:native

Requires [Docker](https://docker.com) to be installed.

## SEE ALSO

**libmagic**(3), **file**(1), **magic**(5)

## LINKS

* [file(1) homepage](https://www.darwinsys.com/file/)
* [Documentation](https://joast.github.io/ruby-filemagic)
* [Source code](https://github.com/joast/ruby-filemagic)
* [RubyGem](https://rubygems.org/gems/ruby-filemagic)
* [Travis CI](https://travis-ci.org/blackwinter/ruby-filemagic)


## AUTHORS

* [Travis Whitton](mailto:tinymountain@gmail.com) (Original author)
* [Jens Wille](mailto:jens.wille@gmail.com) (Previous author up to 0.7.3)
* [Rick Ohnemus](mailto:rick_ohnemus@acm.org) (Current author)


## CREDITS

* [Martin Carpenter](mailto:mcarpenter@free.fr) for Ruby 1.9.2 compatibility
  and other improvements.

* [Pavel Lobashov](@ShockwaveNN) for Dockerfile to build cross-compiled
  Windows extension (pull request #26).


## COPYING

The filemagic extension library is copywrited free software by [Travis
Whitton](mailto:tinymountain@gmail.com). You can redistribute it under the
terms specified in the COPYING file of the Ruby distribution.

## WARRANTY

THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
