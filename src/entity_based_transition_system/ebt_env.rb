require 'optparse'

class EBTSEnv
  attr_reader :action_file

  def initialize
    params = ARGV.getopts '', 'act:'
    @action_file = params['act']
  end
end
