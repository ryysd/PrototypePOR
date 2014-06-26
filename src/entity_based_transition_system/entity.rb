class Entity
  attr_reader :name

  def initialize(name)
    @name = name
  end

  def hash
    @name.hash
  end

  def eql?(other)
    @name == other.name
  end

  def ==(other)
    @name == other.name
  end
end
