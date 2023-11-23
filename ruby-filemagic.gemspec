# -*- encoding: utf-8 -*-
# stub: ruby-filemagic 0.7.3 ruby lib
# stub: ext/filemagic/extconf.rb
# frozen_string_literal: true

Gem::Specification.new do |s|
  s.name = 'ruby-filemagic'
  s.version = '0.7.3'
  s.homepage = 'http://github.com/joast/ruby-filemagic'
  s.licenses = ['Ruby']

  s.metadata = {
    'rubygems_mfa_required' => 'true',
    'homepage_uri' => s.homepage,
    'source_code_uri' => s.homepage,
    'bug_tracker_uri' => "#{s.homepage}/issues",
    'changelog_uri' => "#{s.homepage}/ChangeLog"
  }

  s.require_paths = ['lib']
  s.authors = ['Travis Whitton', 'Jens Wille', 'Rick Ohnemus']
  s.description = 'Ruby bindings to libmagic(3).'
  s.email = 'rick_ohnemus@acm.org'
  s.extensions = ['ext/filemagic/extconf.rb']
  s.extra_rdoc_files = ['README.md', 'ChangeLog', 'ext/filemagic/filemagic.c']

  s.files = [
    'CONTRIBUTING.md',
    'ChangeLog',
    'Dockerfile',
    'README.md',
    'Rakefile',
    'TODO',
    'ext/filemagic/extconf.rb',
    'ext/filemagic/filemagic.c',
    'ext/filemagic/filemagic.h',
    'lib/filemagic.rb',
    'lib/filemagic/ext.rb',
    'lib/filemagic/magic.mgc',
    'lib/filemagic/version.rb',
    'lib/ruby-filemagic.rb',
    'test/excel-example.xls',
    'test/filemagic_test.rb',
    'test/leaktest.rb',
    'test/mahoro.c',
    'test/perl',
    'test/perl.mgc',
    'test/pyfile',
    'test/pyfile-compressed.gz'
  ]

  s.post_install_message = "\nruby-filemagic-0.7.3+ [2023-11-??]:\n\n*** DO NOT USE!!! ***\n\n"

  s.rdoc_options = [
    '--title', 'ruby-filemagic Application documentation (v0.7.3+)',
    '--charset', 'UTF-8', '--line-numbers', '--all', '--main', 'README.md'
  ]

  s.required_ruby_version = '>= 3.0.0'
end
