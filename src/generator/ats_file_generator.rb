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

  def self.transition_relations(petrinet)
    actions = create_actions petrinet
    relations = []

    actions.each do |a|
      actions.each do |b|
	relations.push "#{a.name} s #{b.name}" if a.simulate? b
	relations.push "#{a.name} d #{b.name}" if a.disable? b
      end
    end

    relations
  end

  def self.generate(petrinet)
    transitions = []
    states_num = petrinet.execute do |source, transition, target|
      transitions.push "#{source.to_s}-#{transition.name}->#{target.to_s}"
    end
    Debug.puts_success "number of states: #{states_num}"

    relations = transition_relations petrinet

    JSON.generate ({actions: {relations: relations}, lts: {init: petrinet.init_state.to_s, transitions: transitions}})
  end
end
