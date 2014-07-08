require_relative 'action'
require_relative 'action_table'
require_relative 'word'
require_relative 'vector'
require_relative 'state'
require_relative 'transition'
require_relative 'state_space'
require_relative '../util/debug'

class Reducer
  def initialize(states, actions)
    @states = states
    @actions = actions
    @visited = {}

    after_initialize
  end

  def after_initialize
  end

  def visit(state)
    Debug.puts_error 'nil state is visited.' if state.nil?
    Debug.dputs state.name

    state.reduced = false
    @visited[state.name] = true
  end

  def visited?(state)
    @visited[state.name] || false
  end

  def pre_reduce
    @visited = {}
    @states.each{|s| s.reduced = true}
  end
end
