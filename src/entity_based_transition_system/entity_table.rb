class EntityTable
  def initialize
    @entities = {}
  end

  def generate(name)
    @entities[name] = Entity.new name unless @entities.has_key? name
    @entities[name]
  end
end
