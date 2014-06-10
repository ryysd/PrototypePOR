require 'pp'

class ActionTable
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

class StateSpace
  def initialize
    @states = {}
    @init = nil
  end

  def add(state)
    @states[state.name] = state
  end

  def create(name)
    add State.new name until @states.has_key? name
    @states[name]
  end

  def create!(name)
    throw 'state #{name} is already existing.' if @states.has_key? name
    add State.new name
  end

  def create_as_init(name)
    register_as_init create name
  end

  def register_as_init(state)
    @init = state
  end

  def []=(name, state)
    @states[name] = state
  end

  def [](name)
    @states[name]
  end

  def dump_dot
    stack = [@init]
    visited = []

    puts 'digraph ss {'
    until stack.empty?
      s = stack.pop
      next if visited.include? s
      visited.push s

      s.transitions.each do |t|
	puts "  #{t.src.name} -> #{t.dst.name} [label=\"#{t.action.name}\"];"
	stack.push t.dst
      end
    end
    puts '}'
  end

  def dump
    stack = [@init]
    visited = []

    until stack.empty?
      s = stack.pop
      next if visited.include? s
      visited.push s

      s.successors.each do |succ|
	p "#{s.name} -> #{succ.name}"
	stack.push succ
      end
    end
  end
end

class State
  attr_reader :name, :transitions

  def initialize(name, transitions = [])
    @name = name
    @transitions = transitions
  end

  def successors
    @transitions.map{|t| t.dst}
  end

  def enable_actions
    @transitions.map{|t| t.action}
  end

  def []=(action, dst)
    @transitions.push Transition.new self, action, dst
  end
end

class Transition
  attr_reader :src, :action, :dst

  def initialize(src, action, dst)
    @src = src
    @dst = dst
    @action = action
  end
end

class Action
  attr_reader :name, :simulates, :disables

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
    @actions.each do |a|
      w.each do |b|
	return true if (a.simulate? b) || (b.disable? a)
      end
    end
    false
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
    @actions.each.with_index do |a, idx_a|
      @actions.drop(idx_a+1).each.with_index do |b, idx_b|
	if a.disable? b
	  re_enabled = false
	  v = @actions.slice idx_a, idx_a+idx_b+1
	  v.each do |c|
	    if (a.simulate? c) && (c.simulate? b)
	      re_enabled = true
	      break
	    end
	  end
	  return false until re_enabled
	end
      end
    end

    (0...length).each do |idx|
      break if idx+1 >= length

      v1 = self[0..idx]
      v2 = self[idx+1...length]
      return false if (v2.influence? v1) && (!v1.influence? v2)
    end

    true
  end

  def weak_equal?(w)
    return false if length != w.length
    return self[0] == w[0] if length == 1

    skip = -1
    @actions.each.with_index do |a, idx|
      next if idx == skip
      next if self[idx] == w[idx]

      return false if idx+1 >= length
      return false if !(self[idx+1] == w[idx] && self[idx] == w[idx+1] && !(self[idx].influence? self[idx+1]))
      skip = idx+1
    end

    true
  end

  def weak_prefixes
  end

  def weak_prefix?(w)
    weak_prefixes.include? w
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
    if idx.is_a? Range then Word.new @actions[idx]
    else @actions[idx]
    end
  end
end

class Vector
  def initialize(state, word)
    @state = state
    @word = word
  end
end

class Reducer
  def initialize(states, actions)
    @states = states
    @actions = actions
  end

  def calc_probe_set(v)
  end
end

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

#relations = {
#  x1: {x1: :d , x2: :s , x0: nil, y1: nil, y2: nil, y0: :d},
#  x2: {x1: nil, x2: :d , x0: :s , y1: nil, y2: :d , y0: nil},
#  x0: {x1: :s , x2: nil, x0: :d , y1: nil, y2: :s , y0: :s},
#  y1: {x1: nil, x2: nil, x0: :d , y1: :d , y2: :s , y0: nil},
#  y2: {x1: nil, x2: :d , x0: nil, y1: nil, y2: :d , y0: :s},
#  y0: {x1: nil, x2: :s , x0: :s , y1: :s , y2: nil, y0: :d}
#}
#
#def mk_actions(relations)
#  actions = ActionTable.new
#
#  relations.each{|k, v| actions.create k}
#  relations.each do |k, v|
#    src = actions[k]
#
#    v.each do |nk, nv|
#      dst = actions[nk]
#      case nv
#      when :d then src.disable dst
#      when :s then src.simulate dst
#      when nil
#      end
#    end
#  end
#
#  actions
#end
#actions = mk_actions relations
#
actions = SimulationDisablingFileParser.parse './input/sample.sd'
actions.dump
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
x2x1y1 = x2 + x1 + y1

puts x1x2y1.feasible?
puts x2x1y1.feasible?

puts x1x2.influence? y1y2
puts x1.influence? y1y2

puts x1x2y1.weak_equal? x1y1x2
puts x1x2y1.weak_equal? x2x1y1
p x2.influence? x1

pp (x1y1.prime_cause actions[:y2]).map{|p| p.name}
pp (x1.prime_cause actions[:x2]).map{|p| p.name}
pp (x1x2.prime_cause actions[:x0]).map{|p| p.name}
pp (x1x2.prime_cause actions[:y1]).map{|p| p.name}
pp (x1.prime_cause actions[:y2]).map{|p| p.name}

#action_table = SimulationDisablingFileParser.parse './input/sample.sd'
ss = TransitonFileParser.parse './input/sample.tr', actions
ss.dump_dot
