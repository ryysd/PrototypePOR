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
end
