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
    tmp_actions = {}
    actions = {}
    state_entities = {}
    states = petrinet.execute do |source, transition, target|
      source_entities = (petrinet.places.zip source.marking).select{|p, e| e > 0}.map{|p, e| "#{p.id}_#{e}"}
      target_entities = (petrinet.places.zip target.marking).select{|p, e| e > 0}.map{|p, e| "#{p.id}_#{e}"}

      state_entities[source.to_s] = source_entities
      state_entities[target.to_s] = target_entities

      eraser = source_entities - target_entities
      creator = target_entities - source_entities

      action = Action.new transition.id, creator, eraser
      tmp_actions[action.to_s] = action if tmp_actions[action.to_s].nil?
      index = tmp_actions.select{|action_name, a| a.name == action.name}.length

      action_name = "#{action.name}_#{index}"
      actions[action_name] = Action.new action_name, creator, eraser

      transitions.push "#{source.to_s}-#{action_name}-#{target.to_s}"
      # transitions.push "#{source.to_s}-#{transition.name}-#{target.to_s}"
    end
    Debug.puts_success "number of states: #{states.length}"

    actions = actions.values

    JSON.generate (
      {
	actions: {
	  relations: (action_relations actions),
	  entities: (action_entities actions)
	}, 
	lts: {
	  init: petrinet.init_state.to_s, 
	  transitions: transitions, 
	  states: state_entities}}
    )
  end
end
