class State
  attr_reader :marking

  def initialize(marking)
    @marking = marking
  end

  def successors(incidence_column_vectors, transitions)
    (0...transitions.length).map do |i|
      new_marking = (@marking.zip incidence_column_vectors[i]).map{|(a,b)| a+b}
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
