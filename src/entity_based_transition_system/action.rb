class Action
  attr_reader :creator, :reader, :eraser, :embargoes
  def initialize(name="", c=[], r=[], d=[], n=[])
    @name = name
    @creator = c
    @reader = r
    @eraser = d
    @embargoes = n
  end

  def simulate?(other)
    !(@creator & (other.reader | other.eraser)).empty? ||
      !(@eraser & (other.creator | other.embargoes)).empty?
  end

  def disable?(other)
    !(@eraser & (other.reader | other.eraser)).empty? ||
      !(@creator & (other.creator | other.embargoes)).empty?
  end

  def to_hash
    c = "c_#{(@creator.sort{|x, y| x.name <=> y.name}.map{|ent| ent.name}.join)}"
    r = "r_#{(@reader.sort{|x, y| x.name <=> y.name}.map{|ent| ent.name}.join)}"
    e = "e_#{(@eraser.sort{|x, y| x.name <=> y.name}.map{|ent| ent.name}.join)}"
    n = "n_#{(@embargoes.sort{|x, y| x.name <=> y.name}.map{|ent| ent.name}.join)}"

    [c, r, e, n].join '_'
  end

  def name
    @name = to_hash if @name.empty?
    @name
  end
end
