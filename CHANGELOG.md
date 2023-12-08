# Revision history for ruby-filemagic

## 0.8.0 [2023-11-??]

* Require ruby version 3.0.0 or later.

* Separate error messages for missing header and library in extconf.rb

* Don't use default include and library directories with dir_config

	Using default directories in publicly available gems can cause
	problems because the directories are used when building unless all
	of them are overridden. For this gem it means running "gem
	install" with "--with-magic-dir=..." and "--with-gnurx-dir=..."
	(yes, even if gnurx isn't installed on the system). Kind of
	annoying to have to look at a gem's extconf.rb to see what
	dir_config calls are being made and what needs to be overridden
	because of defaults that conflict with how ruby was
	configured/installed.

	The above discovery was prompted by having libmagic installed in
	both the system default location and a non-standard location (that
	ruby uses for a bunch of other third-party libraries). The
	following warning was issued when trying to use the gem unless the
	gem was installed with both '--with-magic-dir=..." and
	"-with-gnurx-dir=...":

	FileMagic v0.7.3: compiled magic version [5.40] does not match
	with shared library magic version [5.43]

	Note: Checked out MANY gems and very few of then included paths
	for include and library directories. In other words, most gems
	expect who is ever installing the gem to specify directories for
	things that are in non-standard locations.

* Use assert_raises instead of assert_raise

        If minitest_tu_shim is installed, then minitest will be used instead
        of test-unit resulting in deprecation warnings for assert_raise.
        Use assert_raises because it works with both test-unit and minitest.

* Default to using test-unit gem even if minitest_tu_shim is installed

	Force test-unit gem to be used unless "USE_TEST_UNIT=no" is set in
	the environment. If "USE_TEST_UNIT=no" is set in the environment
	and minitest_tu_shim is installed, then minitest will be used
	instead of test-unit. If minitest_tu_shim isn't installed, then
	the environment variable doesn't matter and test-unit will be
	used.

* Fixed "note:" output from some compilers when compiling filemagic.c

	Increased version string temporary buffers from 8 to 16 bytes to
	suppress the following note, and 17 or 18 other lines of output,
	when compiling filemagic.c:

	note: ‘sprintf’ output between 5 and 14 bytes into a destination of size 8

* Create test/pylink if system supports symbolic links.

	Don't package test/pylink (a symbolic link to test/pyfile). Gem
	packaging creates a dangling symbolic link if the symbolic link is
	packaged.  Also, symbolic links might not even be supported on the
	installation system.  Create the link if it is needed when tests
	are run. If the system doesn't support symbolic links, then the
	link won't be created and the symbolic link tests will be skipped.

* Converted README and ChangeLog to markdown.

* Housekeeping.


## 0.7.3 [2022-01-07]

* Dockerfile to build native extension (pull request #26 by Pavel Lobashov).

* Include paths for ARM-based Apple Macs (Apple Silicon) (pull request #35
  by @545ch4).


## 0.7.2 [2017-07-02]

* Fix segfault on `buffer(nil)` when compiled with GCC (pull request #24 by
  Yuya Tanaka).


## 0.7.1 [2015-10-27]

* List default lib and header directories (pull request #18 by Adam Wróbel).


## 0.7.0 [2015-07-28]

* Changed FileMagic#io to optionally rewind input afterwards.

* New method FileMagic.library_version (`magic_version`).

* New method FileMagic#descriptor (`magic_descriptor`).

* New method FileMagic#fd.

* Added new constants from file 5.23.


## 0.6.3 [2015-02-06]

* Fixed build error with Clang (issue #13 by Stan Carver II).


## 0.6.2 [2015-02-06]

* Improved Windows support (issue #10 by Matt Hoyle).

* Fixed FileMagic.path to account for missing magic file.


## 0.6.1 [2014-08-19]

* Account for options passed to FileMagic.fm (issue #7 by Adam Wróbel).


## 0.6.0 [2014-05-16]

* Required Ruby version >= 1.9.3.

* New method FileMagic#list (`magic_list`).

* New method FileMagic#load (`magic_load`).

* New method FileMagic.path (`magic_getpath`).

* New method FileMagic.magic_version.

* New method FileMagic.flags.

* New flag `no_check_builtin`.

* Tests print libmagic version and path.

* Internal refactoring.


## 0.5.2 [2014-04-24]

* Use MAGIC_VERSION if available.

* Made tests more robust.


## 0.5.1 [2014-04-15]

* New method FileMagic#io.

* Housekeeping.


## 0.5.0 [2013-12-19]

* Removed usage of `RARRAY_PTR` (cf. Ruby bug
  [#8399](https://bugs.ruby-lang.org/issues/8399)).

* Housekeeping.


## 0.4.2 [2010-09-16]

* Not all versions/distributions have file/patchlevel.h


## 0.4.1 [2010-09-15]

* Added MAGIC_VERSION

* Fixed example script


## 0.4.0 [2010-09-14]

* Brushed up C layer

  * Moved most of the Ruby stuff to C

  * No longer expose internal state (@closed, @flags)

  * No longer expose internal C methods (fm_*)

  * Updated for magic(4) version 5.04


## 0.3.0 [2010-09-10]

* Ruby 1.9.2 compatibility (Martin Carpenter)
* Exposed flags as symbols (Martin Carpenter)


## 0.2.2 [2010-03-02]
* Allow '.' when abbreviating mime types (Eric Schwartz)
* Cleanup and project file fixes


## 0.2.1 [2008-09-18]
* Added mahoro source file and tests for reference and inspiration
* We have a Rubyforge project now! :-)


## 0.2.0 [2008-09-12]

* Modified C API

  * Uniform C function prefix rb_magic_ (instead of magick_)

  * Uniform Ruby method prefix fm_ (instead of none)

  * More magic(4) constants (if available)

* Added Ruby wrapper for more rubyish interface

* Added extensions for File and String core classes

* Added/updated project files

* Now available as a Rubygem!


## 0.1.1 [2003-07-30]

* Added manual close method

* Added unit test suite


## 0.1.0 [2003-07-28]

*   Initial release.
