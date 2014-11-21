require_relative './petrinet/pnml.rb'
require_relative './conv_env.rb'
require_relative './petrinet2lmntal_converter.rb'
require 'pp'

env = ConvEnv.new
petrinet = PNML.parse env.pnml_file
converter = Petrinet2LMNtalConverter.new
puts converter.convert petrinet
