require_relative '../util/colored_string'

class Debug
  @@enable = false

  def self.enable?
    @@enable
  end

  def self.enable
    @@enable = true
  end

  def self.disable
    @@enable = false
  end

  def self.dputs(text = '')
    puts text if @@enable
  end

  def self.puts_error(text = '')
    puts ColoredString.red text if @@enable
  end

  def self.puts_success(text = '')
    puts ColoredString.green text if @@enable
  end
end
