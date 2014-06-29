require 'optparse'
require_relative '../src/util/debug'

class ScriptEnv
  attr_reader :debug, :png, :pnml_file, :script_path

  def initialize
    params = ARGV.getopts '', 'debug', 'png', 'pnml:'
    @debug = params['debug']
    @png = params['png']
    @pnml_file = params['pnml']
    @script_path = './src'
  end

  tmp_dir = './tmp'
  ['ats', 'full', 'reduced'].each do |name|
    ['json', 'dot', 'png'].each do |ext|
      define_method("#{name}_#{ext}_file"){"#{tmp_dir}/#{name}.#{ext}"}
    end
  end
end

class Option
  def initialize(options)
    @options = options
  end

  def to_s
    @options.map{|k, v| (v == true) ? "--#{k}" : "--#{k} #{v}" if v != false}.join ' '
  end
end

env = ScriptEnv.new
Debug.enable

def execute_script(script, options, message, stdout = nil)
  puts message
  cmd = "ruby #{script} #{(Option.new options).to_s}"
  cmd += " > #{stdout}" unless stdout.nil?

  Debug.puts_success cmd
  puts
  system cmd
end

options = {pnml: env.pnml_file, o: env.ats_json_file}
exit unless execute_script "#{env.script_path}/petrinet/petrinet.rb", options, "generate state space..."

options = {ats: env.ats_json_file, 'full-dot' => env.full_dot_file,  'reduced-dot' => env.reduced_dot_file, debug: env.debug}
exit unless execute_script "#{env.script_path}/por/por.rb", options, 'reduce state space...'

if env.png
  puts "generate #{env.reduced_png_file}..."
  system "dot -Tpng #{env.reduced_dot_file} -o #{env.reduced_png_file}"
  system "open #{env.reduced_png_file}"
end
