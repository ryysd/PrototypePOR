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

  def prime_cause(w)
    def _prime_cause_rec(actions)
      t = actions.pop
      v = (t.influence? self) || (actions.any?{|rest| t.influence? rest}) ? t : nil
      actions.empty? ? [v] : ((_prime_cause_rec actions).push v)
    end

    Word.new (_prime_cause_rec w.actions.clone).compact
  end

  def ==(a)
    @name == a.name
  end

  def to_s
    @name
  end
end
