class ColoredString
  def self.color_string(code, string)
    "\e[#{code}m#{string}\e[0m"
  end

  def self.green(string)
    ColoredString.color_string(32, string)
  end

  def self.red(string)
    ColoredString.color_string(31, string)
  end

  def self.blue(string)
    ColoredString.color_string(34, string)
  end

  def self.yellow(string)
    ColoredString.color_string(33, string)
  end
end
