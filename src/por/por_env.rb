require 'optparse'

class POREnv 
  attr_reader :ats_file, :full_dot_file, :reduced_dot_file, :debug

  def initialize
    params = ARGV.getopts '', 'ats:', 'full-dot:', 'reduced-dot:', 'debug'
    @ats_file = params['ats']
    @full_dot_file = params['full-dot']
    @reduced_dot_file = params['reduced-dot']
    @debug = params['debug']
  end
end
