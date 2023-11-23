# FileMagic Library Binding

## VERSION

This documentation refers to filemagic version 0.7.3+

## DESCRIPTION

FileMagic extension module. See also libmagic(3), file(1), and magic(5).
DO NOT USE!!! This is in the process of being improved.

### Constants

`MAGIC_NONE`
:   No flags
`MAGIC_DEBUG`
:   Turn on debugging
`MAGIC_SYMLINK`
:   Follow symlinks
`MAGIC_COMPRESS`
:   Check inside compressed files
`MAGIC_DEVICES`
:   Look at the contents of devices
`MAGIC_MIME`
:   Return a mime string
`MAGIC_CONTINUE`
:   Return all matches, not just the first
`MAGIC_CHECK`
:   Print warnings to stderr


### Methods

`file(filename)`
:   Returns a textual description of the contents of the filename argument
`buffer(string)`
:   Returns a textual description of the contents of the string argument
`check(filename)`
:   Checks the validity of entries in the database file passed in as filename
`compile(filename)`
:   Compiles the database file passed in as filename
`load(filename)`
:   Loads the database file passed in as filename
`close()`
:   Closes the magic database and frees any memory allocated


### Synopsis

```ruby
    require 'filemagic'

    p FileMagic::VERSION
    # => "0.7.3"
    p FileMagic::MAGIC_VERSION
    # => "5.39"

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

### Environment

The environment variable `MAGIC` can be used to set the default magic file
name.

### Installation

Install the gem:

    sudo gem install ruby-filemagic

The file(1) library and headers are required:

Debian/Ubuntu
:   `libmagic-dev`
Fedora/SuSE
:   `file-devel`
Alpine
:   `libmagic file file-dev`
Gentoo
:   `sys-libs/libmagic`
OS X
:   `brew install libmagic`


### Build native extension

    rake docker:gem:native

Requires [Docker](https://docker.com) to be installed.

## LINKS

* [file(1) homepage](https://www.darwinsys.com/file/)
* [Documentation](https://blackwinter.github.io/ruby-filemagic)
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
