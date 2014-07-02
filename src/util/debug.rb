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

  ['puts', 'print'].each do |name|
    define_singleton_method("d#{name}"){|text=''| send "#{name}", text if enable?}
    define_singleton_method("#{name}_error"){|text=''| send "#{name}", (ColoredString.red text) if enable?}
    define_singleton_method("#{name}_warn"){|text=''| send "#{name}", (ColoredString.yellow text) if enable?}
    define_singleton_method("#{name}_success"){|text=''| send "#{name}", (ColoredString.green text) if enable?}
    define_singleton_method("#{name}_information"){|text=''| send "#{name}", (ColoredString.blue text) if enable?}
  end

  def self.puts_boolean(b)
    b ? (puts_success 'true') : (puts_error 'false')
  end
end

class Dumper
  ['puts', 'print'].each do |name|
    define_singleton_method("d#{name}"){|text=''| send "#{name}", text}
    define_singleton_method("#{name}_error"){|text=''| send "#{name}", (ColoredString.red text)}
    define_singleton_method("#{name}_warn"){|text=''| send "#{name}", (ColoredString.yellow text)}
    define_singleton_method("#{name}_success"){|text=''| send "#{name}", (ColoredString.green text)}
    define_singleton_method("#{name}_information"){|text=''| send "#{name}", (ColoredString.blue text)}
  end

  def self.puts_boolean(b)
    b ? (puts_success 'true') : (puts_error 'false')
  end
end
