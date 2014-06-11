require_relative 'action'
require_relative 'action_table'
require_relative 'word'
require_relative 'vector'
require_relative 'state'
require_relative 'transition'
require_relative 'state_space'

class Reducer
  def initialize(states, actions)
    @states = states
    @actions = actions
  end

  def calc_probe_set(v)
  end
end
