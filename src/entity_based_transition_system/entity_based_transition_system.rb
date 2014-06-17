require 'json'

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
  def initialize(c=[], r=[], d=[], n=[])
    @creator = c
    @reader = r
    @eraser = d
    @embargoes = n
  end

  def simulate?(other)
    !(@creator & (other.reader | other.eraser)).empty? |
      !(@eraser & (other.creator | other.embargoes)).empty
  end

  def disable?(other)
    !(@eraser & (other.reader | other.eraser)).empty? |
      !(@creator & (other.creator | other.embargoes)).empty
  end

  def name
    c = "c_#{(@creator.sort.map{|ent| ent.name}.join)}"
    r = "r_#{(@creator.sort.map{|ent| ent.name}.join)}"
    e = "e_#{(@creator.sort.map{|ent| ent.name}.join)}"
    n = "n_#{(@creator.sort.map{|ent| ent.name}.join)}"

    [c, r, e, n].join '_'
  end
end

class State
  attr_reader :entities
  attr_accessor :visited

  def initialize(entities = [])
    @entities = entities
    @visited = false
  end

  def successor(action)
    State.new @entities - action.eraser + action.creator
  end

  def enable?(action)
    (@entities - (action.reader | action.eraser)).empty? &&
      ((action.embargoes & action.creator) & @entities).empty?
  end

  def enable_actions(actions)
    actions.select{|a| enable? a}
  end

  def successors(actions)
    (enable_actions actions).map{|a| next_state a}
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
end

class StateSpace
  attr_reader :transitions

  def initialize(init, actions)
    @states = []
    @init = init
    @actions = actions
    @transitions = []
  end

  def generate
    work_queue = [@init]

    until work_queue.empty?
      state = work_queue.pop
      state.visited = true

      (state.enable_actions @actions).each do |action|
	succ = state.successor action
	work_queue.push state if !state.visited

	@transitions.push Transition.new state, action, succ
      end
    end
  end
end

class Dumper
  def self.dump_state_space(state_space)
    state_space.transitions.each do |t|
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

json = JSON.load File.read '../input/sample.json'
init = State.new json['init'].map{|e| Entity.new e}
actions = json['actions'].map do |action|
  Action.new action['c'], action['r'], action['d'], action['n']
end

ss = StateSpace.new init, actions
ss.generate

Dumper.dump_state_space ss
Dumper.dump_action_relations actions
