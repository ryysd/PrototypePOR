require 'active_support/core_ext'
require_relative './petrinet_node.rb'
require_relative './place.rb'
require_relative './transition.rb'

class Petrinet
  attr_reader :name, :places, :transitions

  def initialize(name, places, transitions)
    @name = name
    @places = places
    @transitions = transitions
  end
end

class PNML
  def self.pipe_form_pnml?(xml_hash)
    xml_hash['pnml']['net']['type'] == 'P/T net'
  end

  def self.single_element?(element)
    element.size == 1 || (element.instance_of? Hash)
  end

  def self.parse(file_name)
    xml = (File.open file_name).read

    hash =  Hash.from_xml xml
    pipe = pipe_form_pnml? hash

    net = pipe ? hash['pnml']['net'] : hash['pnml']['net']['page']
    value_key = pipe ? 'value' : 'text'

    hash_places = if single_element? net['place'] then [net['place']] else net['place'] end
    hash_transitions = if single_element? net['transition'] then [net['transition']] else net['transition'] end
    places = hash_places.map{|p| Place.new p['id'], (p.has_key? 'name') ? (p['name'][value_key]) : p['id'], (p.has_key? 'initialMarking') ? (p['initialMarking'][value_key].gsub(/[^0-9]/, "").to_i) : 0}
    transitions = hash_transitions.map{|t| Transition.new t['id'], (t.has_key? 'name') ? t['name'][value_key] : t['id']}

    nodes = places + transitions
    net['arc'].each do |a| 
      source_id = a['source'].downcase
      target_id = a['target'].downcase

      source = nodes.find{|n| n.id == source_id}
      target = nodes.find{|n| n.id == target_id}

      case source
      when Place
        target.add_input_place source
      when Transition
        source.add_output_place target
      else
        p 'invalid type'
      end
    end

    name_hash = hash['pnml']['net']['name']
    name = if name_hash.nil? then nil else name_hash['text'] end
    Petrinet.new (name || (File.basename file_name)), places, transitions
  end
end

