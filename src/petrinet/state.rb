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
