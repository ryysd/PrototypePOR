class Action
  attr_reader :name, :creator, :reader, :eraser, :embargoes
  attr_writer :name
  def initialize(name="", c=[], d=[], r=[], n=[])
    @name = name
    @creator = c
    @reader = r
    @eraser = d
    @embargoes = n
  end

  def simulate?(other)
    !(@creator & (other.reader | other.eraser)).empty? # ||
      !(@eraser & (other.creator | other.embargoes)).empty?
  end

  def disable?(other)
    !(@eraser & (other.reader | other.eraser)).empty? ||
      !(@creator & (other.creator | other.embargoes)).empty?
  end

  def to_s
    c = @creator.sort{|a, b| a <=> b}.join ','
    d = @eraser.sort{|a, b| a <=> b}.join ','
    r = @reader.sort{|a, b| a <=> b}.join ','
    n = @embargoes.sort{|a, b| a <=> b}.join ','

    "c_#{c}_d_#{d}_r_#{r}_n_#{n}"
  end
end
