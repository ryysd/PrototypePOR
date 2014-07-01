require 'matrix'
require_relative 'petrinet_node'
require_relative 'transition'
require_relative 'place'
require_relative 'arc'
require_relative 'state'

class Petrinet
  attr_reader :places, :transitions, :incidence_matrix, :input_matrix, :output_matrix, :init_state

  def initialize(places, transitions)
    @places = places
    @transitions = transitions

    @init_state = State.new @places.map{|p| p.initial_marking}
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

  def execute
    states = {}
    work_queue = [@init_state]
    column_vectors = @incidence_matrix.column_vectors.map{|v| v.to_a}

    until work_queue.empty?
      state = work_queue.pop

      unless states.has_key? state.to_s
	states[state.to_s] = state
	Debug.puts_information "#{states.length} states are expanded." if states.length % 10000 == 0
	(state.successors column_vectors, @transitions).each do |trans, succ|
	  work_queue.push succ
	  yield state, trans, succ
	end
      end
    end

    states.length
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
