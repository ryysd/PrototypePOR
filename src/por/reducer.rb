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
    @probe_set = {}
    @visited = []
  end

  def probe_set(v)
    return [] if v.after.enable_actions.empty?

    new_probe_set = []
    @probe_set[v] = [] unless @probe_set.has_key? v

    p = @probe_set[v]
    p.push v.after.enable_actions.first if p.empty?
    p.each{|a| v.after.enable_actions.each{|b| new_probe_set.push b if (b.disable? a) && (!p.include? b)}}

    p += new_probe_set
  end

  def visit(state)
    if state.nil?
      Debug.puts_error 'maybe occur error: nil state is visited.'
      return
    end

    Debug.dputs state.name
    state.reduced = false
    @visited.push state
  end

  def reduce(use_pma = false)
    @states.each{|s| s.reduced = true}
    @visited = []

    work_queue = [(Vector.new @states.init, Word.new([]))]

    until work_queue.empty?
      vector = work_queue.pop
      state = vector.after

      Debug.dputs '----------------------------------------------'
      Debug.dputs "(#{vector.state.name}, #{vector.word.to_s})"
      unless @visited.include? state
	visit state
  
	missed_actions = use_pma ? (vector.potentially_missed_action @actions) : (vector.missed_action @actions)
	missed_actions.each do |vm|
	  v = vm[0...vm.length-1]

	  v.hard_prefix.each do |w|
	    visit vector.state.after w
	  end
	  Debug.dputs "ma: #{vm.to_s}"

	  if !@states.init.enable? vm then Debug.puts_error "#{vm.to_s} is not enable at #{@states.init.name}." 
	  else work_queue.push Vector.new (@states.init), vm
	  end
	end

	Debug.dputs
	Debug.dputs '### probe set ###'
	(probe_set vector).each do |p|
	  work_queue.push Vector.new @states.init, vector.word + p
	  Debug.dputs p.name
	end
      end
      Debug.dputs '----------------------------------------------'
      Debug.dputs
    end

    if Debug.enable?
      reduced = @states.select{|s| s.reduced}
      Debug.puts_success "reduced: #{reduced.length}/#{@states.length} (#{reduced.length.to_f/@states.length}%)"
    end
  end
end
