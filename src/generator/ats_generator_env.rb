require 'optparse'

class ATSGeneratorEnv
  attr_reader :action_file, :dot_file

  def initialize
    params = ARGV.getopts '', 'act:', 'dot:'
    @action_file = params['act']
    @dot_file = params['dot']
  end
end
