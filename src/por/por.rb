require 'pp'
require_relative 'reducer'
require_relative 'por_env'
require_relative '../util/ats_file_reader'
require_relative '../util/colored_string'
require_relative '../util/test_printer'

env = POREnv.new
ats = ATSFileReader.read env.ats_file

actions = ats[:action_table]
state_space = ats[:state_space]

#if env.use_pma?
#  json = JSON.load File.read env.action_file
#  state_space.each{|s|}
#end

Debug.enable if env.debug

unless env.full_dot_file.nil?
  File.open(env.full_dot_file, 'w') do |file|
    file.write state_space.dot Debug.enable?
  end
end

unless env.reduced_dot_file.nil?
  reducer = Reducer.new state_space, actions
  reducer.reduce

  File.open(env.reduced_dot_file, 'w') do |file|
    file.write state_space.dot Debug.enable?
  end
end

#state_space.dump_dot
