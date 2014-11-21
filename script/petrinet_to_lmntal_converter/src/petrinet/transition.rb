class Transition < PetrinetNode
  attr_reader :inputs, :outputs

  def initialize(id, name)
    super id, name
    @inputs = []
    @outputs = []
  end

  def add_input_place(place)
    @inputs.push(place)
  end

  def add_output_place(place)
    @outputs.push(place)
  end
end
