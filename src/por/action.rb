class Action
  attr_reader :name, :simulates, :disables, :creator, :reader, :eraser, :embargoes, :age

  def initialize(name, creator=[], reader=[], eraser=[], embargoes=[])
    @name = name
    @simulates = {}
    @disables = {}

    # for fairness
    @age = 0

    # for pma
    @creator = creator
    @reader = reader
    @eraser = eraser
    @embargoes = embargoes
  end

  def simulate(a)
    @simulates[a.name] = true
  end

  def disable(a)
    @disables[a.name] = true
  end

  def simulate?(a)
    @simulates[a.name] || false
  end

  def disable?(a)
    @disables[a.name] || false
  end

  def influence?(a)
    (simulate? a) || (a.disable? self)
  end

  def independent?(a)
    !simulate? a
  end

  def prime_cause(w)
    rest_flags = Array.new(w.length){true}
    (w.length-1..0).each do |i|
      t = w[i]
      rest_flags[i] = (t.influence? self) || (0...w.length).any?{|j| i != j && rest_flags[j] && (t.influence? w[j])}
    end

    Word.new (w.actions.zip rest_flags).map{|a, rest| rest ? a : nil}.compact
  end

  def inc_age
    @age += 1
  end

  def reset_age
    @age = 0
  end

  def ==(a)
    @name == a.name
  end

  def to_s
    @name
  end
end
