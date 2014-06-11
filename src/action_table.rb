class ActionTable
  include Enumerable

  def initialize
    @actions = {}
  end

  def add(action)
    @actions[action.name] = action
  end

  def create(name)
    add Action.new name until @actions.has_key? name
    @actions[name]
  end

  def create!(name)
    throw 'action #{name} is already existing.' if @actions.has_key? name
    add Action.new name
  end

  def []=(name, action)
    @actions[name] = action
  end

  def [](name)
    @actions[name]
  end

  def each
    @actions.each do |name, action|
      yield action
    end
  end

  def dump_sd
    @actions.each do |name, action|
      action.simulates.each{|target| puts "#{action.name} simulate #{target.name}"}
      action.disables.each{|target| puts "#{action.name} disable #{target.name}"}
    end
  end

  def dump
    order = @actions.map{|k, v| k}

    header = '   '
    order.each{|o| header += "#{o.to_s} "}
    puts header

    order.each do |osrc|
      src = @actions[osrc]

      line = "#{osrc} "
      order.each do |odst|
	dst = @actions[odst]
	if src.simulate? dst then line += '▷' 
	elsif src.disable? dst then line += '◀' 
	else line += ' '
	end
	line += '  '
      end

      puts line
    end
  end
end
