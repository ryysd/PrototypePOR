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

  def influence?(a)
    (simulate? a) || (a.disable? self)
  end

  def ==(a)
    @name == a.name
  end
end

class Word
  include Enumerable

  attr_reader :actions

  def initialize(actions)
    @actions = actions
  end

  def influence?(w)
    ret = false

    @actions.each do |a|
      w.each do |b|
	ret = (a.simulate? b) || (b.disable? a)
	break if ret
      end
    end

    ret
  end

  def prime_cause(a)
    def _prime_cause_rec(a, actions)
      t = actions.pop
      v = (t.influence? a) || (actions.any?{|rest| t.influence? rest}) ? t : nil
      actions.empty? ? [v] : ((_prime_cause_rec a, actions).push v)
    end

    (_prime_cause_rec a, @actions.clone).compact
  end

  def feasible?
  end

  def weak_equal?(w)
  end

  def weak_difference(w)
    words = []

    diff = []
    [0...length].each do |idx|
      if self[idx] == w[idx] && !diff.empty? then words.push diff
      else diff.push self[idx]
      end
    end

    words
  end

  def head(size)
    @actions[0...size]
  end

  def tail(size)
    self[(length-size)...length]
  end

  def length
    @actions.length
  end

  def each
    @actions.each do |action|
      yield action
    end
  end

  def ==(w)
    @actions == w.actions
  end

  def +(w)
    Word.new @actions + w.actions
  end

  def [](idx)
    @actions[idx]
  end
end

class Vector
  def initialize(state, word)
    @state = state
    @word = word
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
x1   = Word.new [actions[:x0]]
x1   = Word.new [actions[:x1]]
x2   = Word.new [actions[:x2]]
y1   = Word.new [actions[:y0]]
y1   = Word.new [actions[:y1]]
y2   = Word.new [actions[:y2]]
x1y1 = x1 + y1
x1x2 = x1 + x2
y1y2 = y1 + y2
x1x2y1 = x1x2 + y1
x1y1x2 = x1y1 + x2

puts x1x2.influence? y1y2
puts x1.influence? y1y2

x1x2y1.weak_equal? x1y1x2

pp (x1y1.prime_cause actions[:y2]).map{|p| p.name}
pp (x1.prime_cause actions[:x2]).map{|p| p.name}
pp (x1x2.prime_cause actions[:x0]).map{|p| p.name}
pp (x1x2.prime_cause actions[:y1]).map{|p| p.name}
pp (x1.prime_cause actions[:y2]).map{|p| p.name}
