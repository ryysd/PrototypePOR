require_relative 'action'
require_relative 'state'
require_relative 'transition'
require_relative 'state_space'
require_relative 'entity'
require_relative 'entity_table'
require_relative 'ebt_env'
require_relative 'dumper'

env = EBTSEnv.new
json = JSON.load File.read env.action_file
entities = EntityTable.new
init = State.new json['init'].map{|e| entities.generate e}
actions = json['actions'].map do |name, set|
  Action.new name, set['c'].map{|e| entities.generate e}, set['r'].map{|e| entities.generate e}, set['d'].map{|e| entities.generate e}, set['n'].map{|e| entities.generate e}
end

ss = StateSpace.new init, actions
ss.generate

Dumper.dump_state_space ss
Dumper.dump_action_relations actions
