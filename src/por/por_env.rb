require 'optparse'

class POREnv 
  attr_reader :ats_file, :full_dot_file, :reduced_dot_file

  def initialize
    params = ARGV.getopts '', 'ats:', 'full-dot:', 'reduced-dot:'
    @ats_file = params['ats']
    @full_dot_file = params['full-dot']
    @reduced_dot_file = params['reduced-dot']
  end
end
