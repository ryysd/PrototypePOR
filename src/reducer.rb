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
    @probe_set = {}
  end

  def probe_set(v)
    new_probe_set = []
    @probe_set[v] = [] unless @probe_set.has_key? v

    p = @probe_set[v]
    p.push v.after.enable_actions.first if p.empty?
    p.each{|a| v.after.enable_actions.each{|b| new_probe_set.push b if (b.disable? a) && (!p.include? b)}}

    p += new_probe_set
  end

  def reduce
  end
end
