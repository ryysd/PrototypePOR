require 'optparse'
require_relative '../util/debug'

class GeneratorEnv
  attr_reader :pnml_file, :output_file, :mode

  def initialize
    params = ARGV.getopts '', 'pnml:', 'out:', 'ats', 'state-space', 'debug'

    @pnml_file = params['pnml']
    @output_file = params['out']
    @mode = params['ats'] ? :ats : (params['state-space'] ? :state_space : :none)
    Debug.enable if params['debug']
  end
end
