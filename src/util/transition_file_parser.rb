class TransitonFileParser
  def self.parse(file_name, actions)
    states = StateSpace.new

    (File.open file_name).readlines.each do |line|
      case line.chomp.strip
      when /(\w+)\s*-\s*(\w+)\s*->\s*(\w+)/ 
	l = states.create $1.to_sym
	r = states.create $3.to_sym
	action = actions.create $2.to_sym
	l[action] = r
      when /init\s*:\s*(\w+)/
	states.create_as_init $1.to_sym
      end
    end

    states
  end
end
