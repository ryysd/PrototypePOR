require 'pp'
require 'active_support/core_ext'

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
end

class Place < PetrinetNode
  attr_reader :initial_marking

  def initialize(id, name, initial_marking)
    super id, name

    @initial_marking = initial_marking
  end
end

class Transition < PetrinetNode
  def initialize(id, name)
    super
  end
end

class Arc
  attr_reader :source, :target, :inscription

  def initialize(source, target, inscription)
    @source = source
    @target = target
    @inscription = inscription
  end
end

class Petrinet
  attr_reader :places, :transitions

  def initialize(places, transitions)
    @places = places
    @transitions = transitions
  end

  def dot
    dot = ['digraph g{']

    (@places + @transitions).map do |node|
      node.arcs.each do |arc|
	dot.push "  #{arc.source.name} -> #{arc.target.name} [label = \"#{arc.inscription}\"];"
	dot.push "  #{node.name} [shape = box];" if node.is_a? Transition
      end
    end

    dot.push '}'
    dot.join "\n"
  end
end

class PNML
  def self.parse(xml)
    hash =  Hash.from_xml xml
    net = hash['pnml']['net']

    places = net['place'].map{|p| Place.new p['id'], p['name']['value'], p['initialMarking']['value'].to_i}
    transitions = net['transition'].map{|t| Transition.new t['id'], t['name']['value']}

    nodes = places + transitions
    net['arc'].each do |a| 
      source_id = a['source']
      target_id = a['target']

      source = nodes.find{|n| n.id == source_id}
      target = nodes.find{|n| n.id == target_id}

      source.add_arc Arc.new source, target, a['inscription']['value'].to_i
    end

    Petrinet.new places, transitions
  end
end

petrinet = PNML.parse (File.open './tmp/pnml.xml').read
puts petrinet.dot
