class StateSpace
  attr_reader :init

  def initialize
    @states = {}
    @transitions = {}
    @init = nil
  end

  def add(state)
    @states[state.name] = state
  end

  def create(name, entities = [])
    add State.new name, [], entities until @states.has_key? name
    @states[name]
  end

  def create!(name, entities = [])
    throw 'state #{name} is already existing.' if @states.has_key? name
    add State.new name, [], entities
  end

  def create_as_init(name, entities = [])
    register_as_init create name, entities
  end

  def register_as_init(state)
    @init = state
  end

  def length
    @states.length
  end

  def []=(name, state)
    @states[name] = state
  end

  def [](name)
    @states[name]
  end

  def each
    @states.each{|name, s| yield s}
  end

  def select
    @states.select{|name, s| yield s}
  end

  def deadlock_states
    @states.select{|name, s| s.transitions.empty?}
  end

  def reachable_deadlock_states
    stack = [@init]
    visited = []

    deadlocks = []

    until stack.empty?
      s = stack.pop
      next if visited.include? s
      visited.push s

      deadlocks.push s if s.transitions.empty?
      s.transitions.each{|t| stack.push t.dst}
    end

    deadlocks
  end

  def dot(label=true)
    stack = [@init]
    visited = []

    data = []

    data.push 'strict digraph {'
    until stack.empty?
      s = stack.pop
      next if visited.include? s
      visited.push s

      data.push "  \"#{s.name}\" [color=red, style=bold]" if s.successors.empty?
      data.push "  \"#{s.name}\" [style=filled, fillcolor=\"#999999\", fontcolor=white]" if s.reduced
      s.transitions.each do |t|
	color = (t.src.reduced || t.dst.reduced) ? 'style=dashed, color="#999999"' : ''
	edge = "\"#{t.src.name}\" -> \"#{t.dst.name}\""
	data.push label ? "  #{edge} [label=\"#{t.action.name}\", #{color}];" : (color.empty? ? "  #{edge};" : "  #{edge} [#{color}];")
	stack.push t.dst
      end
    end
    data.push '}'

    data.join "\n"
  end

  def dump
    stack = [@init]
    visited = []

    until stack.empty?
      s = stack.pop
      next if visited.include? s
      visited.push s

      s.successors.each do |succ|
	p "#{s.name} -> #{succ.name}"
	stack.push succ
      end
    end
  end
end


