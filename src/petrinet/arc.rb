class Arc
  attr_reader :source, :target, :inscription

  def initialize(source, target, inscription)
    @source = source
    @target = target
    @inscription = inscription
  end
end
