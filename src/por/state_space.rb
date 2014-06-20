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

  def create(name)
    add State.new name until @states.has_key? name
    @states[name]
  end

  def create!(name)
    throw 'state #{name} is already existing.' if @states.has_key? name
    add State.new name
  end

  def create_as_init(name)
    register_as_init create name
  end

  def register_as_init(state)
    @init = state
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

  def dot(label=true)
    stack = [@init]
    visited = []

    data = []

    data.push 'strict digraph {'
    until stack.empty?
      s = stack.pop
      next if visited.include? s
      visited.push s

      data.push "  #{s.name} [color=red, style=bold]" if s.successors.empty?
      data.push "  #{s.name} [style=filled, fillcolor=\"#999999\", fontcolor=white]" if s.reduced
      s.transitions.each do |t|
	color = (t.src.reduced || t.dst.reduced) ? 'style=dashed, color="#999999"' : ''
	edge = "#{t.src.name} -> #{t.dst.name}"
	data.push label ? "  #{edge} [label=\"#{t.action.name}\", #{color}];" : "  #{edge} #{t.src.name} -> #{t.dst.name} [#{color}];"
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


