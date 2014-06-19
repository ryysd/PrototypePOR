require 'json'
require_relative './ats_generator_env'
require_relative '../action'
require_relative '../state'
require_relative '../transition'
require_relative '../state_space'
require_relative '../entity'
require_relative '../entity_table'

class ATSFileGenerator
  def self.generate_transitions(transitions)
    transitions.map{|name, t| "#{t.src.name}-#{t.action.name}->#{t.dst.name}"}
  end

  def self.generate_relations(actions)
    relations = []
    actions.each do |a|
      actions.each do |b|
	relations.push "#{a.name} s #{b.name}" if a.simulate? b
	relations.push "#{a.name} d #{b.name}" if a.disable? b
      end
    end

    relations
  end

  def self.generate_state_space(data)
    entities = EntityTable.new
    init = State.new data['init'].map{|e| entities.generate e}
    actions = data['actions'].map do |name, set|
      Action.new name, set['c'].map{|e| entities.generate e}, set['r'].map{|e| entities.generate e}, set['d'].map{|e| entities.generate e}, set['n'].map{|e| entities.generate e}
    end

    state_space = StateSpace.new init, actions
    state_space.generate
    state_space
  end

  def self.generate(data)
    state_space = ATSFileGenerator.generate_state_space data
    relations = ATSFileGenerator.generate_relations state_space.actions
    transitions = ATSFileGenerator.generate_transitions state_space.transitions
    
    JSON.generate ({actions: {relations: relations}, lts: {init: state_space.init.name, transitions: transitions}})
  end
end

env = ATSGeneratorEnv.new
json = JSON.load File.read env.action_file
puts ATSFileGenerator.generate json
