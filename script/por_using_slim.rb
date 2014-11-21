require 'optparse'
require 'pathname'
require_relative '../src/util/debug'

class Env
  attr_reader :pnml_file, :slim_path, :lmntal_file

  def initialize
    params = ARGV.getopts '', 'slim:', 'pnml:'
    @slim_path = params['slim']
    @pnml_file = params['pnml']
    @tmp_dir = "./tmp/#{File.basename @pnml_file, ".*"}"
    @lmntal_file = File.expand_path "#{@tmp_dir}/lmntal.lmn"
  end
end

def execute_command(command)
  Dumper.puts_information command
  system command
end

env = Env.new

execute_command "ruby ./script/petrinet_to_lmntal_converter/src/p2lconv.rb --pnml #{env.pnml_file} > #{env.lmntal_file}"
execute_command "#{env.slim_path} --nd -p1 --por --no-dump #{env.lmntal_file}"
