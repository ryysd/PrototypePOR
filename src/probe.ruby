require 'pp'

class ActionTable
  def initialize
    @actions = {}
  end

  def add(action)
    @actions[action.name] = action
  end

  def [](name)
    @actions[name]
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

class Action
  attr_reader :name

  def initialize(name)
    @name = name
    @simulates = []
    @disables = []
  end

  def simulate(a)
    @simulates.push a
  end

  def disable(a)
    @disables.push a
  end

  def simulate?(a)
    @simulates.include? a
  end

  def disable?(a)
    @disables.include? a
  end
end

class Word
  def initialize(actions)
    @actions = actions
  end

  def influence?
  end
end

relations = {
  x1: {x1: :d , x2: :s , x0: nil, y1: nil, y2: nil, y0: :d},
  x2: {x1: nil, x2: :d , x0: :s , y1: nil, y2: :d , y0: nil},
  x0: {x1: :s , x2: nil, x0: :d , y1: nil, y2: :s , y0: :s},
  y1: {x1: nil, x2: nil, x0: :d , y1: :d , y2: :s , y0: nil},
  y2: {x1: nil, x2: :d , x0: nil, y1: nil, y2: :d , y0: :s},
  y0: {x1: nil, x2: :s , x0: :s , y1: :s , y2: nil, y0: :d}
}

def mk_actions(relations)
  actions = ActionTable.new

  relations.each{|k, v| actions.add Action.new k}
  relations.each do |k, v|
    src = actions[k]

    v.each do |nk, nv|
      dst = actions[nk]
      case nv
      when :d then src.disable dst
      when :s then src.simulate dst
      when nil
      end
    end
  end

  actions
end

def dump_actions(actions)
  order = actions.map{|k, v| k}

  header = '   '
  order.each{|o| header += "#{o.to_s} "}
  puts header

  order.each do |osrc|
    src = actions[osrc]

    line = "#{osrc} "
    order.each do |odst|
      dst = actions[odst]
      if src.simulate? dst then line += '▷' 
      elsif src.disable? dst then line += '◀' 
      else line += ' '
      end
      line += '  '
    end

    puts line
  end
end

actions = mk_actions relations
actions.dump
