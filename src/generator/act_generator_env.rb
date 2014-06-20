require 'optparse'

class ACTGeneratorEnv
  attr_reader :dot_file

  def initialize
    params = ARGV.getopts '', 'dot:'
    @dot_file = params['dot']
  end
end
