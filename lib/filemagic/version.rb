# frozen_string_literal: true

class FileMagic
  module Version # :nodoc:
    MAJOR = 0
    MINOR = 9
    TINY  = 0

    class << self
      # Returns array representation.
      def to_a
        [MAJOR, MINOR, TINY]
      end

      # Short-cut for version string.
      def to_s
        to_a.join('.')
      end
    end
  end

  # FileMagic version.
  VERSION = Version.to_s
end
