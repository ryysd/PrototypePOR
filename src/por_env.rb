require 'optparse'

class POREnv 
  attr_reader :ats_file

  def initialize
    params = ARGV.getopts '', 'ats:'
    pp params
    @ats_file = params['ats']
  end
end
