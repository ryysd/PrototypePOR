class PetrinetNode
  attr_reader :id, :name, :arcs

  def initialize(id, name)
    @id = id
    @name = name
    @arcs = []
  end

  def add_arc(arc)
    @arcs.push arc
  end

  def arc(target)
    @arcs.find{|a| target.id == a.target.id}
  end
end
