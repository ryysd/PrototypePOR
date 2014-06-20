require 'pp'
require 'json'
require 'optparse'
require_relative 'act_generator_env'
require_relative '../entity_based_transition_system/entity'
require_relative '../entity_based_transition_system/state'
require_relative '../entity_based_transition_system/action'

class StateSpace
  attr_reader :init_state 

  def initialize(entities:, init_entities:, actions:, states:, init_state:)
    @entities = entities
    @init_entities = init_entities
    @actions = actions
    @states = states
    @init_state = init_state
  end

  def to_json
    actions = @actions.inject({}){|h, (name, a)| h[name] = {r: a.reader.map{|e| e.name}, c: a.creator.map{|e| e.name}, d: a.eraser.map{|e| e.name}, n: a.embargoes.map{|e| e.name}}; h}
    JSON.generate ({init: @init_entities, actions: actions})
  end
end

class ACTFileGenerator
  def initialize(entity_num:, max_action_num:, init_num:, max_state_num:, max_edge_num_per_state:, max_creator_size:1024, max_reader_size:1024, max_eraser_size:1024, max_embargoes_size:1024)
    @entity_num = entity_num
    @max_action_num = max_action_num
    @init_num = init_num
    @max_state_num = max_state_num
    @max_edge_num_per_state = max_edge_num_per_state
    @max_creator_size = max_creator_size
    @max_reader_size = max_reader_size
    @max_eraser_size = max_eraser_size
    @max_embargoes_size = max_embargoes_size
    @states = {}
    @actions = {}
    @entities = []
  end

  def register_state(state)
    @states[state.name] = state unless @states.has_key? state.name
    @states[state.name]
  end

  def register_action(action)
    @actions[action.name] = action unless @actions.has_key? action.name
    @actions[action.name]
  end

  def expand(state, action)
    register_state state.successor action
  end

  def generate_merge_action(state)
    targets = @states.values.sample 1

    (targets - [state]).each do |target|
      creator = target.entities - state.entities
      eraser = state.entities - target.entities
      reader = state.entities - creator - eraser
      embargoes = @entities - state.entities - creator

      unless (creator | eraser).empty?
	register_action Action.new '', creator.uniq, reader.uniq, eraser.uniq, embargoes.uniq
      end
    end
  end

  def generate
    @entities = (0...@entity_num).map{|e| Entity.new "e#{e}"}
    init_entities = @entities.sample @init_num
    init_state = (register_state State.new init_entities)
    work_queue = [init_state]
    dot = []
    visited = []

    until work_queue.empty? || @actions.length >= @max_action_num
      state = work_queue.pop
      visited.push state.name

      (0...@max_edge_num_per_state).each do |idx|
	reader = state.entities.sample (rand [state.entities.length, @max_reader_size].min) + 1

	rest = state.entities - reader
	eraser = rest.sample (rand [rest.length, @max_eraser_size].min) + 1

	rest = @entities - reader - eraser - state.entities
	embargoes = rest.sample (rand [rest.length, @max_embargoes_size].min) + 1

	rest =  rest - embargoes
	creator = rest.sample (rand [rest.length, @max_creator_size].min) + 1

	unless (creator | eraser).empty?
	  action = register_action Action.new '', creator, reader, eraser, embargoes
	  succ = register_state state.successor action

	  work_queue.push succ unless visited.include? succ.name
	end
      end
    end

    @states.values.each{|s| generate_merge_action s}

    state_space = StateSpace.new entities: @entities, init_entities:  init_entities, states: @states, actions: @actions, init_state: init_state
    {dot: (dot state_space), state_space: state_space}
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

	dot.push "  #{state.name}->#{succ.name};"
      end
    end
    dot.push '}'

    @dot = dot.join "\n"
  end
end

env = ACTGeneratorEnv.new

#generator = ACTFileGenerator.new entity_num: 6, init_num: 0, max_action_num: 100, max_edge_num_per_state: 5, max_state_num: 300
#generator = ACTFileGenerator.new entity_num: 6, init_num: 0, max_action_num: 30, max_edge_num_per_state: 5, max_state_num: 300
generator = ACTFileGenerator.new entity_num: 6, init_num: 0, max_action_num: 10, max_edge_num_per_state: 5, max_state_num: 300
result = generator.generate
state_space = result[:state_space]
dot = result[:dot]

puts state_space.to_json

unless env.dot_file.nil?
  File.open(env.dot_file, 'w') do |file|
    file.write dot
  end
end
