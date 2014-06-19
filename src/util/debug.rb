class Debug
  @@enable = false

  def self.enable
    @@enable = true
  end

  def self.disable
    @@enable = false
  end

  def self.puts(text = '')
    puts text if @@enable
  end
end
