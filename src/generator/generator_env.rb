require 'optparse'
require_relative '../util/debug'

class GeneratorEnv
  attr_reader :pnml_file, :output_file, :mode

  def initialize
    params = ARGV.getopts '', 'pnml:', 'o:', 'ats', 'state-space', 'debug'

    @pnml_file = params['pnml']
    @output_file = params['o']
    @mode = params['ats'] ? :ats : (params['state-space'] ? :state_space : :none)
    Debug.enable if params['debug']
  end
end
