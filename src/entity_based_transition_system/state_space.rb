require 'pp'

class StateSpace
  attr_reader :actions, :transitions, :init

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
