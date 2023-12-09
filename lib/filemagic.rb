# frozen_string_literal: true

require_relative "filemagic/ruby_filemagic.#{RbConfig::CONFIG['DLEXT']}"
require_relative 'filemagic/version'

class FileMagic
  unless ENV['MAGIC_SILENCE_VERSION_CHECK'] || library_version == MAGIC_VERSION
    warn "#{self} v#{VERSION}: compiled magic version [#{MAGIC_VERSION}] " \
         "does not match with shared library magic version [#{library_version}]"
  end

  # :stopdoc:

  # Map flag symbols to their values
  FLAGS_BY_SYM = constants.each_with_object({}) do |flag, flags|
    fs = flag.to_s
    next unless fs.match?(/^MAGIC_(?!PARAM_|VERSION)/)

    flags[fs[6..].downcase.to_sym] = const_get(flag)
  end

  # Map flag values to their names (Integer => :name). A few old flags have a
  # value of 0 because they are only defined for backward compatibility. This
  # map is useless for those because 0 is forced to be :none.
  FLAGS_BY_INT = FLAGS_BY_SYM.invert.update(0 => :none)

  PARAMETERS_BY_SYM = constants.each_with_object({}) do |param, params|
    ps = param.to_s
    next unless ps.start_with?('MAGIC_PARAM_')

    params[ps[6..].downcase.to_sym] = const_get(param)
  end

  PARAMETERS_BY_INT = PARAMETERS_BY_SYM.invert

  # Extract "simple" MIME type.
  SIMPLE_RE = %r{([.\w\/-]+)}

  # :startdoc:

  @fm = {}

  class << self
    # Provide a "magic singleton".
    def fm(*flags)
      options = flags.last.is_a?(Hash) ? flags.pop : {}
      key = [flags = flags(flags), options]
      fm = @fm[key]

      if fm && !fm.closed?
        fm
      else
        @fm[key] = new(flags, options)
      end
    end

    # Clear our instance cache.
    def clear!
      @fm.each_value(&:close).clear
    end

    # Just like #new, but takes an optional block, in which case #close
    # is called at the end and the value of the block is returned.
    def open(*flags)
      fm = new(*flags)

      if block_given?
        begin
          yield fm
        ensure
          fm.close
        end
      else
        fm
      end
    end

    # Just a short-cut to #open with the +mime+ flag set.
    def mime(*flags, &block)
      self.open(:mime, *flags, &block)
    end

    def magic_version(default = MAGIC_VERSION)
      if default == '0'
        user_magic_version || auto_magic_version || [default, 'unknown']
      else
        [default, 'default']
      end
    end

    # :stopdoc:

    private

    def user_magic_version(key = 'MAGIC_VERSION')
      [ENV[key], 'user-specified'] if ENV[key]
    end

    # This is dangerous...
    def auto_magic_version
      [`file -v`[/\d+\.\d+/], 'auto-detected']
    rescue Errno::ENOENT
      nil
    end

    # :startdoc:
  end

  # Set to true for "simple" file information (i.e. the downcased first word
  # of what would normally be returned). For example: "ascii" instead of
  # "ASCII text" or "ruby" instead of "Ruby script, ASCII text".
  attr_writer :simplified

  # Simple output if true, otherwise normal output for current flags.
  def simplified?
    @simplified
  end

  # Return a string describing the data at the current position of an IO
  # object. If rewind is false, then the The position in the IO object will
  # be changed by this method. If rewind is true, then the position will be
  # restored before returning.
  def io(io, length = 1024, rewind = false)
    pos = io.pos if rewind
    buffer(io.read(length))
  ensure
    io.pos = pos if pos
  end

  def fd(fd)
    descriptor(fd.respond_to?(:fileno) ? fd.fileno : fd)
  end

  # call-seq:
  #   inspect => string
  #
  # Returns a string containing a readable representation of +self+.
  def inspect
    super.insert(-2, closed? ? ' (closed)' : '')
  end
end
