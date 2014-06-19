require 'optparse'

class POREnv 
  attr_reader :ats_file

  def initialize
    params = ARGV.getopts '', 'ats:'
    @ats_file = params['ats']
  end
end
