class SimulationDisablingFileParser
  def self.parse(file_name)
    actions = ActionTable.new

    (File.open file_name).readlines.each do |line|
      case line.chomp.strip
      when /(\w+)\s+(simulate|disable)\s+(\w+)/ 
	l = actions.create $1.to_sym
	r = actions.create $3.to_sym
	$2 == 'simulate' ? (l.simulate r) : (l.disable r)
      when /order\s*:\s*([\w,]+)/
	($1.split ',').each{|s| actions.create s.to_sym}
      end
    end

    actions
  end
end
