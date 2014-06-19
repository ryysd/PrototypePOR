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
    return [] if v.after.enable_actions.empty?

    new_probe_set = []
    @probe_set[v] = [] unless @probe_set.has_key? v

    p = @probe_set[v]
    p.push v.after.enable_actions.first if p.empty?
    p.each{|a| v.after.enable_actions.each{|b| new_probe_set.push b if (b.disable? a) && (!p.include? b)}}

    p += new_probe_set
  end

  def reduce
    @states.each{|s| s.reduced = true}
    @actions.each{|a| a.reduced = true}
    visited = []

    work_queue = [(Vector.new @states.init, Word.new([]))]

    until work_queue.empty?
      vector = work_queue.pop
      state = vector.after

      puts '----------------------------------------------'
      puts "(#{vector.state.name}, #{vector.word.to_s})"
      unless visited.include? state
        puts state.name
	visited.push state

	(vector.missed_action @actions).each do |vm|
	  v = vm[0...vm.length-1]

	  v.weak_prefix.each do |w|
	    puts vector.state.after w
	    visited.push vector.state.after w
	  end
	  puts "ma: #{vm.to_s}"
	  #work_queue.push Vector.new (vector.state.after vm), Word.new([])
	  work_queue.push Vector.new (@states.init), vm
	end

	(probe_set vector).each do |p|
	  work_queue.push Vector.new @states.init, vector.word + p
	end
      end
      puts '----------------------------------------------'
      puts
    end
  end
end
