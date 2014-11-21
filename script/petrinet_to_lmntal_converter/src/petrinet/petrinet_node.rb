class PetrinetNode
  attr_reader :id, :name

  def initialize(id, name)
    @id = id.downcase
    @name = name.downcase
  end
end
