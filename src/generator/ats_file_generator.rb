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
	relations.push "#{a.name} s #{b.name}" if a.simulate? b
	relations.push "#{a.name} d #{b.name}" if a.disable? b
      end
    end

    relations
  end

  def self.state_entities(petrinet, states)
    states.inject({}){|e, (name, s)| e[name] = s.marking.map.with_index{|m, i| m > 0 ? petrinet.places[i].name : nil}.compact; e}
  end

  def self.action_entities(actions)
    actions.inject({}){|e, a| e[a.name] = {r: a.reader, c: a.creator, n: a.embargoes, d: a.eraser}; e}
  end

  def self.generate(petrinet)
    transitions = []
    states = petrinet.execute do |source, transition, target|
      transitions.push "#{source.to_s}-#{transition.name}->#{target.to_s}"
    end
    Debug.puts_success "number of states: #{states.length}"

    actions = create_actions petrinet

    JSON.generate (
      {
	actions: {
	  relations: (action_relations actions),
	  entities: (action_entities actions)
	}, 
	lts: {
	  init: petrinet.init_state.to_s, 
	  transitions: transitions, 
	  states: (state_entities petrinet, states)}}
    )
  end
end
