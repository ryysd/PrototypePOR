class ActionTable
  include Enumerable

  def initialize
    @actions = {}
  end

  def add(action)
    @actions[action.name] = action
  end

  def create(name, creator=[], reader=[], eraser=[], embargoes=[])
    add Action.new name, creator, reader, eraser, embargoes until @actions.has_key? name
    @actions[name]
  end

  def create!(name, creator=[], reader=[], eraser=[], embargoes=[])
    throw 'action #{name} is already existing.' if @actions.has_key? name
    add Action.new name, creator, reader, eraser, embargoes
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
	if (src.simulate? dst) && (src.disable? dst) then line += '×'
	elsif src.simulate? dst then line += '▷' 
	elsif src.disable? dst then line += '◀' 
	else line += ' '
	end
	line += '  '
      end

      puts line
    end
  end
end
