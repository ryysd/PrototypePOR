require 'optparse'
require 'fileutils'
require_relative '../src/util/debug'

class ScriptEnv
  attr_reader :debug, :png, :pnml_file, :script_path, :disable_reduction, :use_cache, :output_file, :use_scala, :check_all, :algorithm

  def initialize
    @debug = @@params['debug']
    @png = @@params['png']
    @pnml_file = @@params['pnml']
    @disable_reduction = @@params['disable-reduction']
    @use_cache = @@params['use-cache']
    @output_file = @@params['out']
    @use_scala = @@params['use-scala']
    @check_all = @@params['all']
    @algorithm = @@params['algorithm'] || :probe
    @script_path = './src'
  end

  @@params = ARGV.getopts '', 'debug', 'png', 'pnml:', 'disable-reduction', 'out:', 'use-cache', 'use-scala', 'all', 'algorithm:'
  file_name = File.basename @@params['pnml'], '.*'
  tmp_dir = "./tmp/#{file_name}"
  FileUtils.mkdir_p tmp_dir unless FileTest.exist? tmp_dir
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

def execute_script(script, options, message, bin = 'ruby', stdout = nil)
  Dumper.puts_information message
  cmd = "#{bin} #{script} #{(Option.new options).to_s}"
  cmd += " > #{stdout}" unless stdout.nil?

  Dumper.puts_information cmd
  puts
  system cmd
end

png_source = env.reduced_dot_file
png_file = env.reduced_png_file
if !env.use_cache || !(File.exist? env.ats_json_file)
  if env.disable_reduction
    options = {pnml: env.pnml_file, debug: env.debug, out: env.full_dot_file, 'state-space' => nil}
    png_source = env.full_dot_file
    png_file = env.full_png_file
  else
    options = {pnml: env.pnml_file, debug: env.debug, out: env.ats_json_file, ats: nil}
    png_source = env.reduced_dot_file
    png_file = env.reduced_png_file
  end

  if env.use_scala
    options['worker-num'] = 8
    exit unless execute_script "-jar ./scala/target/petrinet-por-assembly-1.0-SNAPSHOT.jar", options, "generate state space...", 'java'
  else
    exit unless execute_script "#{env.script_path}/generator/generator.rb", options, "generate state space..."
  end
end

options = {ats: env.ats_json_file, 'full-dot' => env.png ? env.full_dot_file : false,  'reduced-dot' => env.png ? env.reduced_dot_file : false, debug: env.debug, 
	   name: (File.basename env.pnml_file, '.*'), o: env.output_file, all: env.check_all, algorithm: env.algorithm}
exit unless execute_script "#{env.script_path}/por/por.rb", options, 'reduce state space...' unless env.disable_reduction

if env.png
  puts "generate #{png_source}..."
  system "dot -Tpng #{png_source} -o #{png_file}"
  system "open #{png_file}"
end

