require 'pp'
require 'json'
require_relative 'action'
require_relative '../petrinet/petrinet'

class ATSFileGenerator
  def self.create_actions(petrinet)
    (petrinet.input_matrix.column_vectors.zip petrinet.output_matrix.column_vectors).map.with_index do |(readers, erasers), i|
      reader = readers.to_a.map.with_index{|r, j| (r > 0) ? petrinet.places[j].id : nil}.compact
      eraser = erasers.to_a.map.with_index{|e, j| (e > 0) ? petrinet.places[j].id : nil}.compact
      Action.new petrinet.transitions[i].name, reader, eraser
    end
  end

  def self.action_relations(actions)
    relations = []

    actions.each do |a|
      actions.each do |b|
	relations.push "#{a.name}-s-#{b.name}" if a.simulate? b
	relations.push "#{a.name}-d-#{b.name}" if a.disable? b
      end
    end

    relations
  end

  # def self.state_entities(petrinet, states)
  #   #states.inject({}){|e, (name, s)| e[name] = s.marking.map.with_index{|m, i| m > 0 ? petrinet.places[i].name : nil}.compact; e}
  #   states.inject({}){|e, (name, s)| e[name] = s.marking.map.with_index{|m, i| m > 0 ? petrinet.places[i].id : nil}.compact; e}
  # end

  def self.action_entities(actions)
    actions.inject({}){|e, a| e[a.name] = {r: a.reader, c: a.creator, n: a.embargoes, d: a.eraser}; e}
  end

  def self.generate(petrinet)
    transitions = []
    actions = {}
    indexes = {}
    name_map = {}
    state_entities = {}
    states = petrinet.execute do |source, transition, target|
      source_entities = (petrinet.places.zip source.marking).select{|p, e| e > 0}.map{|p, e| "#{p.id}_#{e}"}
      target_entities = (petrinet.places.zip target.marking).select{|p, e| e > 0}.map{|p, e| "#{p.id}_#{e}"}
      
      source_guard_entities = (petrinet.places.zip source.marking).select{|p, e| e > 0}.map{|p, e| "@#{p.id}"}
      target_guard_entities = (petrinet.places.zip target.marking).select{|p, e| e > 0}.map{|p, e| "@#{p.id}"}
      forbidden_creator = target_guard_entities - source_guard_entities
      forbidden_eraser = source_guard_entities - target_guard_entities

      state_entities[source.to_s] = source_entities
      state_entities[target.to_s] = target_entities

      eraser = (source_entities - target_entities) | forbidden_eraser
      creator = (target_entities - source_entities) | forbidden_creator

      raw_action = Action.new transition.id, creator, eraser
      tmp_action = Action.new "#{transition.id}_#{raw_action.to_s}", creator, eraser
      unless name_map.has_key? tmp_action.to_s
	indexes[raw_action.to_s] = 0 unless indexes.has_key? raw_action.to_s
	indexes[raw_action.to_s] += 1
      end

      action = Action.new "#{transition.id}_#{indexes[raw_action.to_s]}", creator, eraser
      unless name_map.has_key? tmp_action.to_s
	actions[action.name] = action
	name_map[tmp_action.to_s] = action.name
      end

      # actions[action_name] = Action.new action_name, creator, eraser if actions[action_name].nil?

      se = "[#{source_entities.join ','}]"
      te = "[#{target_entities.join ','}]"
      source.hash = se
      target.hash = te
      transitions.push "#{se}-#{action.name}-#{te}"
      # transitions.push "#{source.to_s}-#{transition.name}-#{target.to_s}"
    end
    Debug.puts_success "number of states: #{states.length}"

    init_guard_entities = (petrinet.places.zip petrinet.init_state.marking).select{|p, e| e > 0}.map{|p, e| "@#{p.id}"}
    actions = actions.values

    JSON.generate (
      {
	actions: {
	  relations: (action_relations actions),
	  entities: (action_entities actions)
	}, 
	lts: {
	  init: petrinet.init_state.to_s, 
	  init_entities: state_entities[petrinet.init_state.to_s] | init_guard_entities,
	  transitions: transitions, 
	  states: state_entities
	}}
    )
  end
end
