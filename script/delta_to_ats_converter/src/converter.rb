require 'json'
require 'pp'

class Link
  attr_reader :attr, :data

  def initialize(obj)
    parse obj
  end

  def parse(obj)
    @attr = obj["attr"]
    @data = obj["data"]
  end
end

class Membrane
  attr_reader :id, :name, :atoms, :membranes

  def initialize(obj)
    parse obj
  end

  def parse(obj)
    @membranes = obj["membranes"].map {|e| Membrane.new e}
    @atoms= obj["atoms"].map {|e| Atom.new e}
    @name = obj["name"]
    @id = obj["id"]
  end
end

class Atom
  attr_reader :id, :links, :name, :parent_mem_id

  def initialize(obj)
    if obj.has_key? 'parent_mem_id'
      @parent_mem_id = obj["parent_mem_id"]
      parse obj["atom"]
    else
      parse obj
    end
  end

  def parse(obj)
    @id= obj["id"]
    @name = obj["name"]
    @links = obj["links"].map {|e| Link.new e}
  end
end

class DeltaMem
  attr_reader :rule_name, :new_atoms, :new_mems, :del_atoms, :del_mems

  def initialize(obj)
    # unsupport DeltaMem size of delta_mems is larger than 1
    raise 'multi delta-mems is unsupported.' if obj.size > 1

    parse obj[0]
  end

  def parse(obj)
    kv = obj.to_a[0]

    @rule_name = kv[0]
    values = kv[1]

    @new_atoms = values["new_atoms"].map {|e| Atom.new e}
    @new_mems  = values["new_mems"].map {|e| Mem.new e}
    @del_atoms = values["del_atoms"].map {|e| Atom.new e}
    @del_mems  = values["del_mems"].map {|e| Mem.new e}
  end
end

class EntityTable
  def initialize
    @table = []
  end

  def create_entity(entity)
    raise 'duplicate id is unsupported.' if @table.include? entity
    @table.push entity
  end

  def remove_entity(entity)
    @table.erase entity
  end

  def clear
    @table.clear
  end
end

class Delta2ATSConverter
  def initialize
    @action_names = {}
  end

  def make_init_state(obj)
    @init = Membrane.new obj.shift
  end

  def make_delta_mems(obj)
    @delta_mems = obj.map {|e| DeltaMem.new e}
  end

  def make_entity_from_atom(atom)
    links = atom.links.map {|l| l.data}.join ','
    "#{atom.id}_#{atom.name}_(#{links})"
  end

  def make_entity_from_membrane(membrane)
    "#{membrane.id}_#{membrane.name}"
  end

  def convert_membrane_to_entity(membrane)
    atom_entities = membrane.atoms.map {|atom| make_entity_from_atom atom}
    membrane_entities = membrane.membranes.map {|mem| convert_membrane_to_entity mem}
    self_entity = make_entity_from_membrane membrane

    (atom_entities + membrane_entities + [self_entity]).flatten
  end

  def convert_atom_to_entity(atom)
    make_entity_from_atom(atom)
  end

  def make_action_name(rule_name, entities)
    unless @action_names.has_key? rule_name
      @action_names[rule_name] = []
      @action_names[rule_name].push entities
    end

    duplications = @action_names[rule_name]
    duplications.map.with_index do |e, idx|
      return "#{rule_name}:#{idx + 1}" if e == entities
    end

    duplications.push entities
    return "#{rule_name}:#{duplications.size}"
  end

  def convert_delta_mem_to_action(delta_mem)
    new_atom_entities = delta_mem.new_atoms.map {|atom| convert_atom_to_entity atom}
    new_mem_entities = delta_mem.new_mems.map {|mem| convert_membrane_to_entity mem}
    del_atom_entities = delta_mem.del_atoms.map {|atom| convert_atom_to_entity atom}
    del_mem_entities = delta_mem.del_mems.map {|mem| convert_membrane_to_entity mem}

    entities = {c: (new_atom_entities + new_mem_entities).sort, d: (del_atom_entities + del_mem_entities).sort, r: [], n: []}
    action_name = make_action_name delta_mem.rule_name, entities

    ["#{action_name}".to_s, entities]
  end

  def convert(json)
    obj = JSON.parse json
    init_state = make_init_state obj
    delta_mems = make_delta_mems obj
    
    actions_entities = delta_mems.map {|delta_mem| convert_delta_mem_to_action delta_mem}

    init_entities_json = {init_entities: (convert_membrane_to_entity init_state)}
    actions_entities_json = {entities: Hash[*actions_entities.flatten]}

    JSON.generate({lts: init_entities_json, actions: actions_entities_json})
  end
end

converter = Delta2ATSConverter.new 
puts converter.convert File.open(ARGV[0]).read
