class Transition
  attr_reader :src, :dst, :action

  def initialize(src, action, dst)
    @src = src
    @dst = dst
    @action = action
  end

  def to_hash
    "#{src.name}-#{action.name}->#{dst.name}"
  end

  def name
    to_hash
  end
end
