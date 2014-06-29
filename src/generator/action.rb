class Action
  attr_reader :name, :creator, :reader, :eraser, :embargoes
  def initialize(name="", c=[], d=[], r=[], n=[])
    @name = name
    @creator = c
    @reader = r
    @eraser = d
    @embargoes = n
  end

  def simulate?(other)
    !(@creator & (other.reader | other.eraser)).empty? #||
      #!(@eraser & (other.creator | other.embargoes)).empty?
  end

  def disable?(other)
    !(@eraser & (other.reader | other.eraser)).empty? #||
      #!(@creator & (other.creator | other.embargoes)).empty?
  end
end
