# frozen_string_literal: true

class FileMagic # :nodoc:
  module Version # :nodoc:
    # :doc:

    # ruby-filemagic major version number.
    MAJOR = 0
    # ruby-filemagic minor version number.
    MINOR = 9
    # ruby-filemagic patchlevel.
    TINY  = 0

    class << self
      # Returns array representation of the ruby-filemagic vesion number.
      def to_a
        [MAJOR, MINOR, TINY]
      end

      # Short-cut for version string.
      def to_s
        to_a.join('.')
      end
    end
  end

  # FileMagic version as a string.
  VERSION = Version.to_s

  # :enddoc:
end
