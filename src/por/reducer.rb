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

  def reduce
    @visited = {}
  end
end

class ProbeReducer < Reducer
  def initialize(states, actions)
    super

    @probe_set = {}
    @missed_actions = {}
  end

  def choose_first_probe_set(enable_actions)
    #enable_actions.first
    enable_actions.sort{|a, b| a.name <=> b.name}.first
    #enable_actions.sort{|a, b| a.name <=> b.name}.max{|a, b| a.age <=> b.age}
  end

  def age_heuristics(probe_set, enable_actions)
    unprobe_set = enable_actions - probe_set
    unprobe_set.each{|a| a.inc_age}
    probe_set.each{|a| a.reset_age}
  end

  def probe_set(v)
    return [] if v.after.enable_actions.empty?

    new_probe_set = []
    @probe_set[v] = [] unless @probe_set.has_key? v

    p = @probe_set[v]
    enable_actions = v.after.enable_actions
    p.push choose_first_probe_set enable_actions if p.empty?
    p.each{|a| v.after.enable_actions.each{|b| new_probe_set.push b if (b.disable? a) && (!p.include? b)}}
    #age_heuristics(p, enable_actions)

    p += new_probe_set
  end

  def reduce(check_all = true)
    super()

    @states.each{|s| s.reduced = true}

    work_queue = [(Vector.new @states.init, Word.new([]))]

    until work_queue.empty?
      vector = work_queue.pop
      state = vector.after

      next if visited? state
      Debug.dputs '----------------------------------------------'
      Debug.dputs "(#{vector.state.name}, #{vector.word.to_s})"

      visit state
      break if !check_all && state.deadlock?

      missed_actions = vector.fresh_missed_action @actions, @missed_actions
      missed_actions.each do |vm|
	v = vm[0...vm.length-1]

	Debug.dputs "ma: #{vm.to_s}"
	if !@states.init.enable? vm 
	  Debug.puts_error "#{vm.to_s} is not enable at #{@states.init.name}." 
	  next
	end

	v.hard_prefix.each do |w|
	  visit vector.state.after w
	end

	new_vec = Vector.new (@states.init), vm
	work_queue.push new_vec
      end

      Debug.dputs
      Debug.dputs '### probe set ###'
      (probe_set vector).each do |p|
	new_vec = Vector.new @states.init, vector.word + p
	work_queue.push new_vec
	Debug.dputs p.name
      end
    end
    Debug.dputs '----------------------------------------------'
    Debug.dputs

    @states
  end
end
