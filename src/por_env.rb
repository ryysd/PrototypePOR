require 'optparse'

class POREnv 
  attr_reader :relation_file, :lts_file

  def initialize
    params = ARGV.getopts '', 'rel:', 'lts:'
    pp params
    @relation_file = params['rel']
    @lts_file = params['lts']
  end
end
