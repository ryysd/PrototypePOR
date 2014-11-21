require 'optparse'

class ConvEnv 
  attr_reader :pnml_file

  def initialize
    params = ARGV.getopts '', 'pnml:'
    @pnml_file = params['pnml']
  end
end
