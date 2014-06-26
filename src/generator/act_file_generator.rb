require 'pp'
require 'json'
require 'optparse'
require_relative 'act_generator_env'
require_relative '../entity_based_transition_system/entity'
require_relative '../entity_based_transition_system/state'
require_relative '../entity_based_transition_system/action'
require_relative '../entity_based_transition_system/state_space'

#class StateSpace
#  attr_reader :init_state 
#
#  def initialize(entities:, init_entities:, actions:, states:, init_state:)
#    @entities = entities
#    @init_entities = init_entities
#    @actions = actions
#    @states = states
#    @init_state = init_state
#  end
#
#  def to_json
#    actions = @actions.inject({}){|h, (name, a)| h["a#{h.length}"] = {r: a.reader.map{|e| e.name}, c: a.creator.map{|e| e.name}, d: a.eraser.map{|e| e.name}, n: a.embargoes.map{|e| e.name}}; h}
#    JSON.generate ({init: @init_entities, actions: actions})
#  end
#end

class ACTFileGenerator
  def initialize(action_num_per_state:, max_creator_num:1024, max_eraser_num:1024)
    #@entity_num = entity_num
    #@max_action_num = max_action_num
    #@init_num = init_num
    #@max_state_num = max_state_num
    #@max_edge_num_per_state = max_edge_num_per_state
    @max_creator_num = max_creator_num
    @max_eraser_num = max_eraser_num
    @action_num_per_state = action_num_per_state

    @states = {}
    @actions = {}
    #@entities = []
  end

  def register_state(state)
    @states[state.name] = state unless @states.has_key? state.name
    @states[state.name]
  end

  def register_action(action)
    @actions[action.name] = action unless @actions.has_key? action.name
    @actions[action.name]
  end

  #def expand(state, action)
  #  register_state state.successor action
  #end

  #def generate_merge_action(state)
  #  targets = @states.values.sample 1

  #  (targets - [state]).each do |target|
  #    creator = target.entities - state.entities
  #    eraser = state.entities - target.entities
  #    reader = state.entities - creator - eraser
  #    embargoes = @entities - state.entities - creator

  #    unless (creator | eraser).empty?
  #      register_action Action.new '', creator.uniq, reader.uniq, eraser.uniq, embargoes.uniq
  #    end
  #  end
  #end

  #def remove_duplicate_edges
  #  clone = @actions.clone

  #  @actions.select do |a|
  #    clone.each do |b|
  #      a_c = a.creator.sort.map{|c| c.name}
  #      a_d = a.eraser.sort.map{|d| d.name}

  #      b_c = b.creator.sort.map{|c| c.name}
  #      b_d = b.eraser.sort.map{|d| d.name}

  #      (a_c != b_c) || (a_d != b_d)
  #    end
  #  end
  #end

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
      #puts "#{state.name}->#{target.name} [label=\"#{action.name}\"];"
    end
  end

  def generate_actions(entities, states, force_link = true)
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
	state = states.find{|s| valid_target? s, target}
	generate_action state, target, entities unless state.nil?
      end
    end

    @actions
  end

  def generate(state_num)
    entity_num = ((Math.log state_num) / (Math.log 2.0)).ceil
    entities = (0...entity_num).map{|e| Entity.new "e#{e}"}
    states = generate_states entities, state_num
    actions = generate_actions entities, states, true

    init_state = states[0]
    #state_space = StateSpace.new entities: entities, init_entities:  init_state.entities, states: states, actions: actions, init_state: init_state
    StateSpace.new init_state, actions.values
    #{dot: (dot state_space), state_space: state_space}

    #states.each{|s| pp s.name}

    #init_entities = @entities.sample @init_num
    #init_state = (register_state State.new init_entities)
    #work_queue = [init_state]
    #dot = []
    #visited = []
  end

  #def generate
  #  @entities = (0...@entity_num).map{|e| Entity.new "e#{e}"}
  #  init_entities = @entities.sample @init_num
  #  init_state = (register_state State.new init_entities)
  #  work_queue = [init_state]
  #  dot = []
  #  visited = []

  #  until work_queue.empty? || @actions.length >= @max_action_num
  #    state = work_queue.pop
  #    visited.push state.name

  #    (0...@max_edge_num_per_state).each do |idx|
  #      reader = state.entities.sample (rand [state.entities.length, @max_reader_size].min)

  #      rest = state.entities - reader
  #      eraser = rest.sample (rand [rest.length, @max_eraser_size].min)

  #      rest = @entities - reader - eraser - state.entities
  #      embargoes = rest.sample (rand [rest.length, @max_embargoes_size].min)

  #      rest =  rest - embargoes
  #      creator = rest.sample (rand [rest.length, @max_creator_size].min)

  #      unless (creator | eraser).empty?
  #        action = register_action Action.new '', creator, reader, eraser, embargoes
  #        succ = register_state state.successor action

  #        work_queue.push succ unless visited.include? succ.name
  #      end
  #    end
  #  end

  #  @states.values.each{|s| generate_merge_action s}

  #  state_space = StateSpace.new entities: @entities, init_entities:  init_entities, states: @states, actions: @actions, init_state: init_state
  #  {dot: (dot state_space), state_space: state_space}
  #end

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

#generator = ACTFileGenerator.new entity_num: 6, init_num: 0, max_action_num: 100, max_edge_num_per_state: 5, max_state_num: 300
#generator = ACTFileGenerator.new entity_num: 6, init_num: 0, max_action_num: 30, max_edge_num_per_state: 5, max_state_num: 300
#generator = ACTFileGenerator.new entity_num: 6, init_num: 0, max_action_num: 10, max_edge_num_per_state: 5, max_state_num: 300
#generator = ACTFileGenerator.new entity_num: 6, init_num: 0, max_action_num: 8, max_edge_num_per_state: 3, max_state_num: 300, max_embargoes_size: 0, max_reader_size: 0, max_eraser_size: 0
generator = ACTFileGenerator.new action_num_per_state: 3, max_creator_num: 1, max_eraser_num: 0
#generator = ACTFileGenerator.new entity_num: env.entity_num, init_num: env.init_num, max_action_num: env.max_action_num, max_edge_num_per_state: env.max_edge_num_per_state, max_state_num: env.max_state_num, max_creator_size: env.max_creator_num, max_embargoes_size: env.max_embargoes_num, max_reader_size: env.max_reader_num, max_eraser_size: env.max_eraser_num

state_space = generator.generate 50
dot = state_space.generate
#state_space = result[:state_space]
#dot = result[:dot]

puts state_space.to_json

unless env.dot_file.nil?
  File.open(env.dot_file, 'w') do |file|
    file.write dot
  end
end
