require_relative 'lib/filemagic/version'

begin
  require 'hen'

  Hen.lay! {{
    gem: {
      name:        %q{ruby-filemagic},
      version:     FileMagic::VERSION,
      summary:     'Ruby bindings to libmagic(3)',
      authors:     ['Travis Whitton', 'Jens Wille', 'Rick Ohnemus'],
      email:       'rick_ohnemus@acm.org',
      license:     %q{Ruby},
      homepage:    :joast,
      extension:   {
        with_cross_gnurx: lambda { |dir| [dir] },
        with_cross_magic: lambda { |dir| [src =
          File.join(dir, 'src'), File.join(src, '.libs')] }
      },

      required_ruby_version: '>= 3.0.0'
    },
    rdoc: {
      rdoc_files: %w[README.md CHANGELOG.md CODE_OF_CONDUCT.md lib/**/*.rb ext/**/*.c],
      title:      '{name:%s } documentation{version: (v%s)}',
      main:       'README.md'
    }
  }}
rescue LoadError => err
  warn "Please install the `hen' gem. (#{err})"
end

namespace :docker do
  name = "ruby-filemagic-gem-native:#{FileMagic::VERSION}"

  task :build do
    sh *%W[docker build -t #{name} .]
  end

  task :run do
    sh *%W[docker run -it --rm -v #{Dir.pwd}/pkg:/ruby-filemagic/pkg #{name}]
  end

  desc "Build native gems using docker image #{name}"
  task 'gem:native' => %w[build run]
end
