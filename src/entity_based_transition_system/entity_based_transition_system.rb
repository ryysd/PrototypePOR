require 'json'
require 'pp'

class EntityTable
  def initialize
    @entities = {}
  end

  def generate(name)
    @entities[name] = Entity.new name unless @entities.has_key? name
    @entities[name]
  end
end

class Entity
  attr_reader :name

  def initialize(name)
    @name = name
  end

  def ==(other)
    @name = other.name
  end
end

class Action
  attr_reader :creator, :reader, :eraser, :embargoes
  def initialize(name="", c=[], r=[], d=[], n=[])
    @name = name
    @creator = c
    @reader = r
    @eraser = d
    @embargoes = n
  end

  def simulate?(other)
    !(@creator & (other.reader | other.eraser)).empty? |
      !(@eraser & (other.creator | other.embargoes)).empty?
  end

  def disable?(other)
    !(@eraser & (other.reader | other.eraser)).empty? |
      !(@creator & (other.creator | other.embargoes)).empty?
  end

  def to_hash
    c = "c_#{(@creator.sort{|x, y| x.name <=> y.name}.map{|ent| ent.name}.join)}"
    r = "r_#{(@reader.sort{|x, y| x.name <=> y.name}.map{|ent| ent.name}.join)}"
    e = "e_#{(@eraser.sort{|x, y| x.name <=> y.name}.map{|ent| ent.name}.join)}"
    n = "n_#{(@embargoes.sort{|x, y| x.name <=> y.name}.map{|ent| ent.name}.join)}"

    [c, r, e, n].join '_'
  end

  def name
    @name = to_hash if @name.empty?
    @name
  end
end

class State
  attr_reader :entities
  attr_accessor :visited

  def initialize(entities = [])
    @name = ''
    @entities = entities
    @visited = false
  end

  def successor(action)
    State.new @entities - action.eraser + action.creator
  end

  def enable?(action)
    ((action.reader | action.eraser) - @entities).empty? &&
      ((action.embargoes | action.creator) & @entities).empty?
  end

  def enable_actions(actions)
    actions.select{|a| enable? a}
  end

  def successors(actions)
    (enable_actions actions).map{|a| next_state a}
  end

  def to_hash
    "#{@entities.sort{|x, y| x.name <=> y.name}.map{|ent| ent.name}.join}"
  end

  def name
    @name = to_hash if @name.empty?
    @name = 'empty' if @name.empty?
    @name
  end

  def ==(other)
    @entities == other.entities
  end
end

class Transition
  attr_reader :src, :dst, :action

  def initialize(src, action, dst)
    @src = src
    @dst = dst
    @action = action
  end

  def to_hash
    "#{src.name}-#{action.name}->#{dst.name}"
  end

  def name
    to_hash
  end
end

class StateSpace
  attr_reader :transitions

  def initialize(init, actions)
    @states = {}
    @init = init
    @actions = actions
    @transitions = {}
  end

  def register_state(state)
    @states[state.name] = state unless @states.has_key? state.name
    @states[state.name]
  end

  def expand(state, action)
    register_state state.successor action
  end

  def generate
    work_queue = [(register_state @init)]

    until work_queue.empty?
      state = work_queue.pop
      state.visited = true

      (state.enable_actions @actions).each do |action|
	succ = expand state, action
	work_queue.push succ unless succ.visited

	transition = Transition.new state, action, succ
	unless @transitions.has_key? transition.name
	  @transitions[transition.name] = transition
	  #puts "#{state.name}->#{succ.name} [label=\"#{action.name}\"];"
	end
      end
    end
  end
end

class Dumper
  def self.dump_state_space(state_space)
    state_space.transitions.each do |name, t|
      puts "#{t.src.name}-#{t.action.name}->#{t.dst.name}"
    end
  end

  def self.dump_action_relations(actions)
    actions.each do |a|
      actions.each do |b|
	puts "#{a.name} simulate #{b.name}" if a.simulate? a
	puts "#{a.name} disable #{b.name}" if a.disable? a
      end
    end
  end
end

json = JSON.load File.read './input/sample.ent.json'
entities = EntityTable.new
init = State.new json['init'].map{|e| entities.generate e}
actions = json['actions'].map do |name, set|
  Action.new name, set['c'].map{|e| entities.generate e}, set['r'].map{|e| entities.generate e}, set['d'].map{|e| entities.generate e}, set['n'].map{|e| entities.generate e}
end

ss = StateSpace.new init, actions
ss.generate

Dumper.dump_state_space ss
Dumper.dump_action_relations actions
