class Action
  attr_reader :name, :simulates, :disables

  def initialize(name)
    @name = name
    @simulates = []
    @disables = []
  end

  def simulate(a)
    @simulates.push a
  end

  def disable(a)
    @disables.push a
  end

  def simulate?(a)
    @simulates.include? a
  end

  def disable?(a)
    @disables.include? a
  end

  def influence?(a)
    (simulate? a) || (a.disable? self)
  end

  def independent?(a)
    !simulate? a
  end

  def ==(a)
    @name == a.name
  end
end
