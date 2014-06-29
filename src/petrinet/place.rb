class Place < PetrinetNode
  attr_reader :initial_marking

  def initialize(id, name, initial_marking)
    super id, name

    @initial_marking = initial_marking
  end
end
