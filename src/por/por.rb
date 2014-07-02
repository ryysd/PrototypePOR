require 'pp'
require_relative 'reducer'
require_relative 'por_env'
require_relative '../util/ats_file_reader'
require_relative '../util/colored_string'

env = POREnv.new
ats = ATSFileReader.read env.ats_file

actions = ats[:action_table]
state_space = ats[:state_space]

Debug.enable if env.debug

unless env.full_dot_file.nil?
  File.open(env.full_dot_file, 'w') do |file|
    file.write state_space.dot Debug.enable?
  end
end

reducer = Reducer.new state_space, actions
states = reducer.reduce

unless env.reduced_dot_file.nil?
  File.open(env.reduced_dot_file, 'w') do |file|
    file.write state_space.dot Debug.enable?
  end
end

deadlock_states = state_space.deadlock_states
reachable_deadlock_states = state_space.reachable_deadlock_states
Dumper.puts_success 'check validity -------------------------------------------'
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
