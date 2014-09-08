class State
  attr_reader :marking
  attr_writer :hash

  def initialize(marking)
    @marking = marking
    @hash = ''
  end

  def successors(incidence_column_vectors, transitions)
    (0...transitions.length).map do |i|
      valid = true
      new_marking = (@marking.zip incidence_column_vectors[i]).map{|(a,b)| e=a+b; valid=valid&&e>=0; e}
      valid ? [transitions[i], (State.new new_marking)] : nil
    end.compact
  end

  def ==(other)
    @marking == other.marking
  end

  def to_s
    @hash = @marking.to_a.to_s.gsub ' ', '' if @hash.empty?
    @hash
  end
end
