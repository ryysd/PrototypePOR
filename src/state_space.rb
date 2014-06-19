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

  def dump_dot
    stack = [@init]
    visited = []

    puts 'digraph ss {'
    until stack.empty?
      s = stack.pop
      next if visited.include? s
      visited.push s

      s.transitions.each do |t|
	puts "  #{t.src.name} -> #{t.dst.name} [label=\"#{t.action.name}\"];"
	stack.push t.dst
      end
    end
    puts '}'
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


