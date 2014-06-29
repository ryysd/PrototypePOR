require 'optparse'

class ATSFileGeneratorEnv
  attr_reader :pnml_file, :ats_file

  def initialize
    params = ARGV.getopts '', 'pnml:', 'o:'

    @pnml_file = params['pnml']
    @ats_file = params['o']
  end
end
