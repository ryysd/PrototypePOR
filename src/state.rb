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

  def []=(action, dst)
    @transitions.push Transition.new self, action, dst
  end
end
