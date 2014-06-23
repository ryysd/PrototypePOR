class Action
  attr_reader :name, :simulates, :disables, :creator, :reader, :eraser, :embargoes

  def initialize(name, creator=[], reader=[], eraser=[], embargoes=[])
    @name = name
    @simulates = []
    @disables = []

    # for pma
    @creator = creator
    @reader = reader
    @eraser = eraser
    @embargoes = embargoes
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
