# frozen_string_literal: true

gem 'test-unit' unless ENV.fetch('USE_TEST_UNIT', '').casecmp('no').zero?

require 'test/unit'
require 'filemagic'
require 'fileutils'

class TestFileMagic < Test::Unit::TestCase # :nodoc:
  magic_version, origin = FileMagic.magic_version
  MAGIC_VERSION = magic_version.delete('.').to_i

  warn <<~INFO

  magic version: #{magic_version}#{" (#{origin})" if origin}
  library version: #{FileMagic.library_version}
  magic file from #{FileMagic.path}

  INFO

  # remove perl.mgc if it exists
  def self.rm_perl_mgc
    pmp = File.join(File.dirname(__FILE__), 'perl.mgc')
    FileUtils.rm_f(pmp)
  end

  def self.startup
    rm_perl_mgc
  end

  def self.shutdown
    rm_perl_mgc
  end

  def test_file
    fm = FileMagic.new(FileMagic::MAGIC_NONE)

    python_script = match_version(
        0 => 'a python script, ASCII text executable',
      511 => 'Python script, ASCII text executable'
    )

    res = fm.file(path_to('pyfile'))
    assert_equal(python_script, res)

    # The following block ensures that symlinks are only tested on systems
    # that support them.
    begin
      assert(File.writable?('test'), "can't write to test directory")
      File.symlink('pyfile', 'test/pylink')

      if File.symlink?(path_to('pylink'))
        res = fm.file(path_to('pylink'))
        assert_equal(match_version(
            0 => "symbolic link to `pyfile'",
          522 => 'symbolic link to pyfile'
        ), res.strip)
      end

      fm.close
      fm = FileMagic.new(FileMagic::MAGIC_SYMLINK)

      res = fm.file(path_to('pylink'))
      assert_equal(python_script, res)

      fm.close
      fm = FileMagic.new(FileMagic::MAGIC_SYMLINK | FileMagic::MAGIC_MIME)

      res = fm.file(path_to('pylink'))
      assert_equal('text/plain; charset=us-ascii', res)
    rescue NotImplementedError
      # ignore
    ensure
      FileUtils.rm_f('test/pylink')
    end

    fm.close

    fm = FileMagic.new(FileMagic::MAGIC_COMPRESS)

    res = fm.file(path_to('pyfile-compressed.gz'))
    gzip_compressed = 'gzip compressed data, was "pyfile-compressed"'

    if Gem.win_platform?
      assert_match(/^#{gzip_compressed}/, res)
    else
      assert_match(/^#{python_script} \(#{gzip_compressed}/, res)
    end

    fm.close
  end

  def test_buffer
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = fm.buffer("#!/bin/sh\n")
    fm.close
    assert_equal('POSIX shell script, ASCII text executable', res)
  end

  def test_nil_buffer
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    assert_raises(TypeError) { fm.buffer(nil) }
    fm.close
  end

  def test_descriptor
    fm = FileMagic.new(FileMagic::MAGIC_NONE)

    python_script = match_version(
        0 => 'a python script, ASCII text executable',
      511 => 'Python script, ASCII text executable'
    )

    fd_for('pyfile') do |fd|
      res = fm.descriptor(fd)
      assert_equal(python_script, res)
    end

    # The following block ensures that symlinks are only tested on systems
    # that support them.
    begin
      assert(File.writable?('test'), "can't write to test directory")
      File.symlink('pyfile', 'test/pylink')

      if File.symlink?(path_to('pylink'))
        fd_for('pylink') do |fd|
          res = fm.descriptor(fd)
          assert_equal(python_script, res.strip)
        end
      end

      fm.close
      fm = FileMagic.new(FileMagic::MAGIC_SYMLINK)

      fd_for('pylink') do |fd|
        res = fm.descriptor(fd)
        assert_equal(python_script, res)
      end

      fm.close
      fm = FileMagic.new(FileMagic::MAGIC_SYMLINK | FileMagic::MAGIC_MIME)

      fd_for('pylink') do |fd|
        res = fm.descriptor(fd)
        assert_equal('text/plain; charset=us-ascii', res)
      end
    rescue NotImplementedError
      # ignore
    ensure
      FileUtils.rm_f('test/pylink')
    end

    fm.close
    fm = FileMagic.new(FileMagic::MAGIC_COMPRESS)

    fd_for('pyfile-compressed.gz') do |fd|
      res = fm.descriptor(fd)
      gzip_compressed = 'gzip compressed data, was "pyfile-compressed"'

      if Gem.win_platform?
        assert_match(/^#{gzip_compressed}/, res)
      else
        assert_match(/^#{python_script} \(#{gzip_compressed}/, res)
      end
    end

    fm.close
  end

  def test_check
    return if Gem.win_platform?

    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = silence_stderr { fm.check(path_to('perl')) }
    fm.close
    assert(res)
  end

  def test_compile
    assert(File.writable?('test'), "can't write to test directory")

    Dir.chdir(File.dirname(__FILE__)) do
      fm = FileMagic.new(FileMagic::MAGIC_NONE)
      res = fm.compile(path_to('perl'))
      fm.close
      assert(res)
    end
  end

  def test_compile_check
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = fm.check(path_to('perl.mgc'))
    fm.close
    assert(res)
  end

  def test_check_old_magic
    return if MAGIC_VERSION <= 509

    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = silence_stderr { fm.check(path_to('old-perl.mgc')) }
    fm.close
    assert(match_version(
        0 => res,
      539 => !res
    ))
  end

  def test_block
    block_fm = nil

    res = FileMagic.open(FileMagic::MAGIC_NONE) do |fm|
      block_fm = fm
      fm.file(path_to('pyfile'))
    end

    assert_equal(match_version(
        0 => 'a python script, ASCII text executable',
      511 => 'Python script, ASCII text executable'
    ), res)

    assert_predicate(block_fm, :closed?)
  end

  def test_flags_to_int
    assert_raises(TypeError) { FileMagic.flags(0) }
    assert_equal(0, FileMagic.flags([FileMagic::MAGIC_NONE]))
    assert_equal(0, FileMagic.flags([:none]))
    assert_equal(0, FileMagic.flags([]))
    assert_equal(1072, FileMagic.flags(%i[mime continue]))
  end

  def test_setflags
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    assert_empty(fm.flags)
    fm.flags = FileMagic::MAGIC_SYMLINK
    assert_equal([:symlink], fm.flags)
    fm.close
  end

  def test_abbr
    fm = FileMagic.new(:mime, :continue)
    assert_equal(%i[mime_type continue mime_encoding], fm.flags)
    fm.flags = :symlink
    assert_equal([:symlink], fm.flags)
    fm.close
  end

  def test_close
    fm = FileMagic.new
    fm.close
    assert_predicate(fm, :closed?)
    fm.close
    assert_predicate(fm, :closed?)
  end

  def test_getparam
    fm = FileMagic.new
    return unless fm.respond_to?(:getparam)

    assert_raises(TypeError) { fm.getparam([1]) }
    assert_raises(TypeError) { fm.getparam(param: 8192) }
    assert_raises(ArgumentError) { fm.getparam(:bogus) }
    assert_raises(ArgumentError) { fm.getparam(8192) }

    # getparam default value tests
    [
      [ FileMagic::MAGIC_PARAM_BYTES_MAX, :param_bytes_max, 7340032 ],
      [ FileMagic::MAGIC_PARAM_ELF_NOTES_MAX, :param_elf_notes_max, 256 ],
      [ FileMagic::MAGIC_PARAM_ELF_PHNUM_MAX, :param_elf_phnum_max, 2048 ],
      [ FileMagic::MAGIC_PARAM_ELF_SHNUM_MAX, :param_elf_shnum_max, 32768 ],
      [ FileMagic::MAGIC_PARAM_ELF_SHSIZE_MAX, :param_elf_shsize_max, 134217728 ],
      [ FileMagic::MAGIC_PARAM_ENCODING_MAX, :param_encoding_max, 65536 ],
      [ FileMagic::MAGIC_PARAM_INDIR_MAX, :param_indir_max, 50 ],
      [ FileMagic::MAGIC_PARAM_NAME_MAX, :param_name_max, 50 ],
      [ FileMagic::MAGIC_PARAM_REGEX_MAX, :param_regex_max, 8192 ],
    ].each do |pc, ps, value|
      if defined?(pc)
        assert_equal(value, fm.getparam(pc))
        assert_equal(value, fm.getparam(ps))
      end
    end
  end

  def test_c_file
    fm = FileMagic.new
    fm.flags = FileMagic::MAGIC_NONE
    assert_equal(match_version(
        0 => 'ASCII C program text',
      511 => 'C source, ASCII text, with very long lines (331)'
    ), fm.file(path_to('../ext/filemagic/filemagic.c')))
  end

  def test_c_mime_file
    fm = FileMagic.new
    fm.flags = FileMagic::MAGIC_MIME
    assert_equal('text/x-c; charset=us-ascii',
                 fm.file(path_to('../ext/filemagic/filemagic.c')))
  end

  def test_c_buffer
    fm = FileMagic.new
    fm.flags = FileMagic::MAGIC_NONE
    assert_equal(match_version(
        0 => 'ASCII C program text',
      511 => 'C source, ASCII text, with very long lines (331)'
    ), fm.buffer(File.read(path_to('../ext/filemagic/filemagic.c'))))
  end

  def test_c_mime_buffer
    fm = FileMagic.new
    fm.flags = FileMagic::MAGIC_MIME
    assert_equal('text/x-c; charset=us-ascii',
                 fm.buffer(File.read(path_to('../ext/filemagic/filemagic.c'))))
  end

  def test_valid
    fm = FileMagic.new
    assert_predicate(fm, :valid?, 'Default database was not valid.')
  end

  # test abbreviating mime types

  def test_abbrev_mime_type
    fm = FileMagic.mime
    refute_predicate(fm, :simplified?)

    # TODO: need to check file version to know what is expected
    # assert_equal('text/plain; charset=us-ascii', fm.file(path_to('perl')))
    assert_equal('text/x-file; charset=us-ascii', fm.file(path_to('perl')))

    fm.simplified = true
    assert_predicate(fm, :simplified?)

    # TODO: ditto
    # assert_equal('text/plain', fm.file(path_to('perl')))
    assert_equal('text/x-file', fm.file(path_to('perl')))
    assert_equal(match_version(
        0 => 'application/vnd.ms-office',
      511 => 'application/msword',
      514 => 'application/vnd.ms-office',
      539 => 'application/vnd.ms-excel'
    ), fm.file(path_to('excel-example.xls')))
  end

  def test_singleton
    fm1 = FileMagic.fm
    assert_equal(fm1, FileMagic.fm)

    refute_predicate(fm1, :simplified?)
    assert_equal('ASCII text', fm1.file(path_to('a-text-file')))

    fm2 = FileMagic.fm(:mime)
    assert_equal(fm2, FileMagic.fm(:mime))
    refute_equal(fm2, fm1)

    refute_predicate(fm2, :simplified?)
    assert_equal('text/plain; charset=us-ascii', fm2.file(path_to('a-text-file')))

    fm3 = FileMagic.fm(:mime, simplified: true)
    assert_equal(fm3, FileMagic.fm(:mime, simplified: true))
    refute_equal(fm3, fm2)
    refute_equal(fm3, fm1)

    assert_predicate(fm3, :simplified?)
    assert_equal('text/plain', fm3.file(path_to('a-text-file')))
  end

  # utility methods:

  def path_to(file, dir = File.dirname(__FILE__))
    File.join(dir, file)
  end

  def fd_for(file)
    File.open(path_to(file)) { |f| yield f.fileno }
  rescue Errno::EBADF => e
    warn e
  end

  def silence_stderr
    previous_stderr = $stderr.dup
    $stderr.reopen(::File::NULL, mode: 'w')
    yield
  ensure
    $stderr.reopen(previous_stderr)
  end

  def match_version(versions)
    versions.sort_by { |k,| -k }.find { |k,| k <= MAGIC_VERSION }.last
  end
end
