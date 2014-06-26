class State
  attr_reader :entities
  attr_accessor :visited

  def initialize(entities = [])
    @name = ''
    @entities = entities
    @visited = false
  end

  def successor(action)
    State.new @entities - action.eraser + action.creator
  end

  def enable?(action)
    ((action.reader | action.eraser) - @entities).empty? &&
      ((action.embargoes | action.creator) & @entities).empty?
  end

  def enable_actions(actions)
    actions.select{|a| enable? a}
  end

  def successors(actions)
    (enable_actions actions).map{|a| next_state a}
  end

  def to_hash
    "#{@entities.sort{|x, y| x.name <=> y.name}.map{|ent| ent.name}.join}"
  end

  def hash
    @to_hash.hash
  end

  def eql?(other)
    @name == other.name
  end

  def name
    @name = to_hash if @name.empty?
    @name = 'empty' if @name.empty?
    @name
  end

  def ==(other)
    @name == other.name
  end
end
