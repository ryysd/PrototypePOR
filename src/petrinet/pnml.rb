require 'pp'
require 'matrix'
require 'active_support/core_ext'

class PetrinetNode
  attr_reader :id, :name, :arcs

  def initialize(id, name)
    @id = id
    @name = name
    @arcs = []
  end

  def add_arc(arc)
    @arcs.push arc
  end

  def arc(target)
    @arcs.find{|a| target.id == a.target.id}
  end
end

class Place < PetrinetNode
  attr_reader :initial_marking

  def initialize(id, name, initial_marking)
    super id, name

    @initial_marking = initial_marking
  end
end

class Transition < PetrinetNode
  def initialize(id, name)
    super
  end
end

class Arc
  attr_reader :source, :target, :inscription

  def initialize(source, target, inscription)
    @source = source
    @target = target
    @inscription = inscription
  end
end

class State
  attr_reader :marking

  def initialize(marking)
    @marking = marking
  end

  def successors(incidence_matrix, transitions)
    (0...transitions.length).map do |i|
      arr = Array.new transitions.length, 0
      arr[i] = 1

      new_marking = @marking + incidence_matrix * (Vector.elements arr)
      new_marking.all?{|m| m >= 0} ? [transitions[i], (State.new new_marking)] : nil
    end.compact
  end

  def ==(other)
    @marking == other.marking
  end

  def to_s
    @marking.to_a.to_s.gsub ' ', ''
  end
end

class Petrinet
  attr_reader :places, :transitions

  def initialize(places, transitions)
    @places = places
    @transitions = transitions

    @initial_marking = Vector.elements @places.map{|p| p.initial_marking}
    @incidence_matrix, @input_matrix, @output_matrix = create_matrix
  end

  def create_matrix
    row_length = @places.length
    col_length = @transitions.length
    input_matrix_arr = (Array.new row_length).map{Array.new col_length}
    output_matrix_arr = (Array.new row_length).map{Array.new col_length}

    @places.each.with_index do |place, i|
      @transitions.each.with_index do |transition, j|
	input_arc = transition.arc place
	output_arc = place.arc transition

	input_matrix_arr[i][j] = input_arc.nil? ? 0 : input_arc.inscription
	output_matrix_arr[i][j] = output_arc.nil? ? 0 : output_arc.inscription
      end
    end

    input_matrix = Matrix.rows input_matrix_arr
    output_matrix = Matrix.rows output_matrix_arr
    [input_matrix - output_matrix, input_matrix, output_matrix]
  end

  def simulate
    states = []
    work_queue = [(State.new @initial_marking)]

    until work_queue.empty?
      state = work_queue.pop

      states.push state
      (state.successors @incidence_matrix, @transitions).each do |trans, succ|
	work_queue.push succ unless states.include? succ
	yield state, trans, succ
      end
    end
  end

  def csv
    col = @transitions.map{|t| t.name}
    row = @places.map{|p| p.name}

    incidence_csv = [((['incidence'] + col).join ',')]
    input_csv     = [((['input'] + col).join ',')]
    output_csv    = [((['output'] + col).join ',')]

    incidence_csv += @incidence_matrix.row_vectors.map.with_index{|v, i| (v.to_a.unshift row[i]).join ','}
    input_csv += @input_matrix.row_vectors.map.with_index{|v, i| (v.to_a.unshift row[i]).join ','}
    output_csv += @output_matrix.row_vectors.map.with_index{|v, i| (v.to_a.unshift row[i]).join ','}

    [incidence_csv, input_csv, output_csv].map{|c| c.join "\n"}
  end

  def dot
    dot = ['digraph g{']

    (@places + @transitions).map do |node|
      node.arcs.each do |arc|
	dot.push "  #{arc.source.name} -> #{arc.target.name} [label = \"#{arc.inscription}\"];"
	dot.push "  #{node.name} [shape = box];" if node.is_a? Transition
      end
    end

    dot.push '}'
    dot.join "\n"
  end
end

class PNML
  def self.parse(xml)
    hash =  Hash.from_xml xml
    net = hash['pnml']['net']

    places = net['place'].map{|p| Place.new p['id'], p['name']['value'], p['initialMarking']['value'].to_i}
    transitions = net['transition'].map{|t| Transition.new t['id'], t['name']['value']}

    nodes = places + transitions
    net['arc'].each do |a| 
      source_id = a['source']
      target_id = a['target']

      source = nodes.find{|n| n.id == source_id}
      target = nodes.find{|n| n.id == target_id}

      source.add_arc Arc.new source, target, a['inscription']['value'].to_i
    end

    Petrinet.new places, transitions
  end
end

petrinet = PNML.parse (File.open './tmp/pnml.xml').read
petrinet.simulate do |source, transition, target|
  puts "\"#{source.to_s}\" -> \"#{target.to_s}\" [label=\"#{transition.name}\"];"
end

#incidence_csv, input_csv, output_csv = petrinet.csv

#File.open './tmp/incidence.csv', 'w' do |file|
#  file.write incidence_csv
#end
#
#File.open './tmp/input.csv', 'w' do |file|
#  file.write input_csv
#end
#
#File.open './tmp/output.csv', 'w' do |file|
#  file.write output_csv
#end
#puts petrinet.dot
