class Transition
  attr_reader :src, :action, :dst

  def initialize(src, action, dst)
    @src = src
    @dst = dst
    @action = action
  end
end
