require 'pp'
require_relative 'transition'

class StateSpace
  attr_reader :actions, :transitions, :init, :dot, :states

  def initialize(init, actions)
    @states = {}
    @init = init
    @actions = actions
    @transitions = {}
    @dot = []
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
    dot = []

    dot.push 'strict digraph {'
    until work_queue.empty?
      state = work_queue.pop
      state.visited = true

      (state.enable_actions @actions).each do |action|
	succ = expand state, action
	work_queue.push succ unless succ.visited

	transition = Transition.new state, action, succ
	unless @transitions.has_key? transition.name
	  @transitions[transition.name] = transition
	  #dot.push "  #{state.name}->#{succ.name} [label=\"#{action.name}\"];"
	  dot.push "  #{state.name}->#{succ.name};"
	end
      end
    end
    dot.push '}'

    @dot = dot.join "\n"
  end

  def to_json
    actions = @actions.inject({}){|h, a| h["a#{h.length}"] = {r: a.reader.map{|e| e.name}, c: a.creator.map{|e| e.name}, d: a.eraser.map{|e| e.name}, n: a.embargoes.map{|e| e.name}}; h}
    JSON.generate ({init: @init.entities, actions: actions})
  end
end
