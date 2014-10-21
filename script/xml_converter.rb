require 'active_support/core_ext'
require "rexml/document"
require 'optparse'
require 'pp'

class ScriptEnv
  attr_reader :pnml_file

  def initialize
    @pnml_file = @@params['pnml']
  end

  @@params = ARGV.getopts '', 'pnml:'
end

class XMLConverter
  def self.pipe_form_pnml?(doc)
    doc.elements['pnml/net'].attributes['type'] == 'P/T net'
  end

  def self.to_pnml(doc)
    return doc.to_s if !pipe_form_pnml? doc

    net_element = doc.elements['pnml/net']
    page_element = REXML::Element.new "page"
    page_element.add_attribute 'id', 'page0'
    net_element.elements.each do |elem|
      case elem.name
      when 'place' then
      when 'transition' then
        elem.elements.delete_all 'rate'
        elem.elements.delete_all 'timed'
        elem.elements.delete_all 'orientation'
        elem.elements.delete_all 'priority'
        elem.elements.delete_all 'initialServer'
      when 'arc' then
        elem.elements.delete_all 'tagged'
        elem.elements.delete_all 'arcpath'
        elem.elements.delete_all 'type'
      end
      page_element.add_element elem
    end

    new_net_element = doc.elements['pnml/net']
    new_net_element.attributes['type'] = 'http://www.pnml.org/version-2009/grammar/ptnet'
    new_net_element.elements.delete_all '*'
    new_net_element.add_element page_element
    # page_element.add_element doc.elements['pnml/net']

    doc.elements.delete 'xml'
    doc.elements.delete 'pnml/net'
    doc.elements['pnml'].add_element new_net_element

    new_xml = doc.to_s
    return ((new_xml.gsub /\n+/, "\n").gsub 'value', 'text').gsub 'Default,', ''
  end
end

env = ScriptEnv.new
# XMLConverter.to_pnml (File.open env.pnml_file).read
puts XMLConverter.to_pnml REXML::Document.new(open(env.pnml_file))
