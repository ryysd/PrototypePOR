require 'pp'
require 'json'

class EntityGenerator
  def initialize(entity_num:, action_num:, init_num:, max_state_num:, max_edge_num_per_state:)
    @entity_num = entity_num
    @action_num = action_num
    @init_num = init_num
    @max_state_num = max_state_num
    @max_edge_num_per_state
  end

  def generate
    entities = (0...@entity_num).map{|e| "e#{e}"}
    init = entities.sample @init_num
  end
end

#class EntityGenerator
#  def self.generate(entity_num, action_num, init_num, max_set_size)
#    entities = (0...entity_num).map{|e| "e#{e}"}
#    init = entities.sample init_num
#
#    actions = {}
#    (0...action_num).each do |idx|
#      creator = entities.sample (rand [entities.length, max_set_size[:creator]].min) + 1
#
#      tmp = entities - creator
#      reader = tmp.sample (rand [tmp.length, max_set_size[:reader]].min) + 1
#
#      tmp = tmp - reader
#      eraser = tmp.sample (rand [tmp.length, max_set_size[:eraser]].min) + 1
#
#      tmp = tmp - eraser
#      embargoes = tmp.sample (rand [tmp.length, max_set_size[:embargoes]].min) + 1
#
#      actions["a#{idx}"] = {c: creator, r: reader, d: eraser, n: embargoes}
#    end
#
#    {init: init, actions: actions}
#  end
#end

puts JSON.generate EntityGenerator.generate 10, 1000, 3, {creator: 5, reader: 3, eraser: 3, embargoes: 2}
