require 'optparse'

class POREnv 
  attr_reader :ats_file, :full_dot_file, :reduced_dot_file, :debug, :name, :output_file, :check_all, :algorithm

  def initialize
    params = ARGV.getopts '', 'ats:', 'full-dot:', 'reduced-dot:', 'name:', 'o:', 'debug', 'all', 'algorithm:'
    @ats_file = params['ats']
    @full_dot_file = params['full-dot']
    @reduced_dot_file = params['reduced-dot']
    @debug = params['debug']
    @name = params['name']
    @output_file = params['o']
    @check_all = params['all']
    @algorithm = params['algorithm'] || :probe
  end
end
