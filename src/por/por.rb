require 'pp'
require 'json'
require_relative 'reducer'
require_relative 'por_env'
require_relative '../util/ats_file_reader'
require_relative '../util/colored_string'
require_relative '../util/simple_timer'

env = POREnv.new
ats = ATSFileReader.read env.ats_file

actions = ats[:action_table]
state_space = ats[:state_space]

Debug.enable if env.debug

unless env.full_dot_file.nil?
  File.open(env.full_dot_file, 'w') do |file|
    file.write state_space.dot
  end
end

timer = SimpleTimer.new true
reducer = Reducer.new state_space, actions
states = reducer.reduce
execution_time = timer.stop
Dumper.puts_success "execution time: #{execution_time} ms"
Dumper.dputs

unless env.reduced_dot_file.nil?
  File.open(env.reduced_dot_file, 'w') do |file|
    file.write state_space.dot
  end
end

deadlock_states = state_space.deadlock_states
reachable_deadlock_states = state_space.reachable_deadlock_states
Dumper.puts_success 'validation -----------------------------------------------'
Dumper.puts_success "deadlock states            : #{deadlock_states.length}"
Dumper.puts_success "deadlock states (reachable): #{reachable_deadlock_states.length}"
Dumper.print_success "valid: "; Dumper.puts_boolean (deadlock_states.length == reachable_deadlock_states.length)
Dumper.puts_success '-----------------------------------------------------------'
Dumper.dputs

reduced = states.select{|s| s.reduced}

Dumper.puts_success 'number of states ------------------------------------------'
Dumper.puts_success "full   : #{states.length}"
Dumper.puts_success "reduced: #{states.length - reduced.length}"
Dumper.puts_success "reduction rate: #{reduced.length*100.0/states.length}%"
Dumper.puts_success '-----------------------------------------------------------'

json = JSON.generate({
  name: env.name,
  execution_time: execution_time,
  validation: {
    deadlock_states: deadlock_states.length,
    reachable_deadlock_states: reachable_deadlock_states.length,
    valid: deadlock_states.length == reachable_deadlock_states.length
  },
  result: {
    full_state_space: states.length,
    reduced_state_space: states.length,
    reduction_rate: (reduced.length*100.0/states.length)
  }
})

unless env.output_file.nil?
  File.open(env.output_file, 'w') do |file|
    file.write json
  end
end
