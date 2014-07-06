class State
  attr_reader :name, :transitions, :entities
  attr_accessor :reduced

  def initialize(name, transitions = [], entities = [])
    @name = name
    @transitions = transitions
    @reduced = false

    @entities = entities # for pma
  end

  def successors
    @transitions.map{|t| t.dst}
  end

  def enable_actions
    @transitions.map{|t| t.action}
  end

  def successor(action)
    transition = @transitions.find{|t| t.action == action}
    transition.nil? ? nil : transition.dst
  end

  def after(w)
    return successor w if w.is_a? Action
    return self if w.empty?

    s = successor w.first
    s.nil? ? nil : (s.after w.drop 1)
  end

  def over_approximation_after(w)
    @entities + w.reduce([]){|ca, a| ca + a.creator}
  end

  def weak_enable?(w, a)
    entities = over_approximation_after w
    ((a.reader | a.eraser) - entities).empty?
  end

  def enable?(w)
    !(after w).nil?
  end

  def deadlock?
    @transitions.empty?
  end

  def []=(action, dst)
    @transitions.push Transition.new self, action, dst
  end
end
