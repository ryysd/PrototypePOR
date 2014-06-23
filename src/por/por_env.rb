require 'optparse'

class POREnv 
  attr_reader :ats_file, :full_dot_file, :reduced_dot_file, :debug, :use_pma

  def initialize
    params = ARGV.getopts '', 'ats:', 'full-dot:', 'reduced-dot:', 'debug', 'use-pma', 'act:'
    @ats_file = params['ats']
    @full_dot_file = params['full-dot']
    @reduced_dot_file = params['reduced-dot']
    @debug = params['debug']
    @use_pma = params['use-pma']
    @act_file = params['act']
  end
end
