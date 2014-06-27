require 'pp'
require 'json'
require 'optparse'
require_relative 'act_generator_env'
require_relative '../entity_based_transition_system/entity'
require_relative '../entity_based_transition_system/state'
require_relative '../entity_based_transition_system/action'
require_relative '../entity_based_transition_system/state_space'

class ACTFileGenerator
  def initialize(action_num_per_state:, max_creator_num:1024, max_eraser_num:1024)
    @max_creator_num = max_creator_num
    @max_eraser_num = max_eraser_num
    @action_num_per_state = action_num_per_state

    @states = {}
    @actions = {}
  end

  def register_state(state)
    @states[state.name] = state unless @states.has_key? state.name
    @states[state.name]
  end

  def register_action(action)
    @actions[action.name] = action unless @actions.has_key? action.name
    @actions[action.name]
  end

  def generate_states(entities, state_num)
    (0...state_num).map do |idx|
      state_entities = []
      (idx.to_s 2).reverse.each_byte.with_index do |b, bit_idx|
	state_entities.push entities[bit_idx] if b.chr == '1'
      end

      register_state State.new state_entities
    end
  end

  def generate_action(state, target, entities, strict = false)
    creator = target.entities - state.entities
    eraser = state.entities - target.entities
    reader = strict ? (state.entities - creator - eraser) : []
    embargoes = strict ? (entities - state.entities) : []

    unless (creator | eraser).empty?
      register_action Action.new '', creator.uniq, reader.uniq, eraser.uniq, embargoes.uniq
    end
  end

  def generate_actions(entities, states, force_link = false)
    linked = []

    def valid_target?(state, target)
      creator = target.entities - state.entities
      eraser = state.entities - target.entities

      (state != target) && (creator.length <= @max_creator_num) && (eraser.length <= @max_eraser_num)
    end

    states.each do |state|
      target_candidates = states.select{|target| valid_target? state, target}

      tmp_targets = target_candidates.partition{|target| linked.include? target.name}
      linked_targets = tmp_targets[0]
      not_linked_targets = tmp_targets[1]

      action_num_per_state = (rand @action_num_per_state) + 1
      targets = not_linked_targets.sample action_num_per_state
      targets += linked_targets.sample (action_num_per_state - targets.length) if targets.length < action_num_per_state

      targets.each do |target|
	linked.push target.name unless linked.include? target.name
	generate_action state, target, entities
      end
    end

    if force_link
      not_linked_states = states.select{|s| !(linked.include? s.name)}

      not_linked_states.each do |target|
        state = states.select{|s| valid_target? s, target}.sample
        generate_action state, target, entities unless state.nil?
      end
    end

    @actions
  end

  def generate(state_num)
    entity_num = ((Math.log state_num) / (Math.log 2.0)).ceil
    entities = (0...entity_num).map{|e| Entity.new "e#{e}"}
    states = generate_states entities, state_num
    actions = generate_actions entities, states,

    init_state = states[0]
    StateSpace.new init_state, actions.values
  end

  def dot(state_space)
    work_queue = [(register_state state_space.init_state)]
    visited = []
    dot = []

    dot.push 'strict digraph {'
    until work_queue.empty?
      state = work_queue.pop
      visited.push state.name

      (state.enable_actions @actions.values).each do |action|
	succ = expand state, action
	work_queue.push succ unless visited.include? succ.name

	#dot.push "  #{state.name}->#{succ.name} [label=\"#{action.name}\"];"
	dot.push "  #{state.name}->#{succ.name};"
      end
    end
    dot.push '}'

    @dot = dot.join "\n"
  end
end

env = ACTGeneratorEnv.new

generator = ACTFileGenerator.new action_num_per_state: 3, max_creator_num: 1, max_eraser_num: 0
state_space = generator.generate 50
dot = state_space.generate

puts state_space.to_json

unless env.dot_file.nil?
  File.open(env.dot_file, 'w') do |file|
    file.write dot
  end
end
