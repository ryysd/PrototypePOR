class State
  attr_reader :name, :transitions

  def initialize(name, transitions = [])
    @name = name
    @transitions = transitions
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
    return self if w.empty?

    s = successor w.first
    s.nil? ? nil : (s.after w.drop 1)
  end

  def enable?(w)
    !(after w).nil?
  end

  def []=(action, dst)
    @transitions.push Transition.new self, action, dst
  end
end
