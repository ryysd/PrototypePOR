require 'optparse'
require 'fileutils'
require_relative '../src/util/debug'

class ScriptEnv
  attr_reader :debug, :png, :pnml_file, :script_path, :disable_reduction, :use_cache, :output_file, :use_scala, :use_cpp, :check_all, :algorithm, :use_tina, :source_path

  def initialize
    @debug = @@params['debug']
    @png = @@params['png']
    @pnml_file = @@params['pnml']
    @disable_reduction = @@params['disable-reduction']
    @use_cache = @@params['use-cache']
    @output_file = @@params['out']
    @use_scala = @@params['use-scala']
    @use_cpp = @@params['use-cpp']
    @use_tina = @@params['use-tina']
    @check_all = @@params['all']
    @algorithm = @@params['algorithm'] || :probe
    @script_path = './script'
    @source_path = './src'
  end

  @@params = ARGV.getopts '', 'debug', 'png', 'pnml:', 'disable-reduction', 'out:', 'use-cache', 'use-scala', 'all', 'algorithm:', 'use-cpp', 'use-tina'
  file_name = File.basename @@params['pnml'], '.*'
  tmp_dir = "./tmp/#{file_name}"
  FileUtils.mkdir_p tmp_dir unless FileTest.exist? tmp_dir
  ['ats', 'full', 'reduced', 'tina', 'formatted'].each do |name|
    ['txt', 'json', 'dot', 'png', 'pnml'].each do |ext|
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

exit unless execute_script "#{env.script_path}/xml_converter.rb", {pnml: env.pnml_file}, "format pnml file...", 'ruby', env.formatted_pnml_file

png_source = env.reduced_dot_file
png_file = env.reduced_png_file
if !env.use_cache || !(File.exist? env.ats_json_file)
  if env.disable_reduction
    options = {pnml: env.formatted_pnml_file, debug: env.debug, out: env.full_dot_file, 'state-space' => nil}
    png_source = env.full_dot_file
    png_file = env.full_png_file
  else
    options = {pnml: env.formatted_pnml_file, debug: env.debug, out: env.ats_json_file, ats: nil}
    png_source = env.reduced_dot_file
    png_file = env.reduced_png_file
  end

  if env.use_scala
    options['worker-num'] = 1
    exit unless execute_script "-jar ./scala/target/petrinet-por-assembly-1.0-SNAPSHOT.jar", options, "generate state space...", 'java'
  elsif env.use_cpp
    options.delete :ats
    options.delete :pnml
    # options[:debug] = nil if env.debug
    options[:file] = env.formatted_pnml_file
    exit unless execute_script "./c/build/petrinet", options, "generate state space...", ''
  elsif env.use_tina
    cmd = "tina -PNML -g < #{env.formatted_pnml_file} > #{env.tina_txt_file}"
    Dumper.puts_information cmd
    exit unless system cmd

    options.delete :ats
    options.delete :pnml
    options[:tina] = true
    # options[:debug] = nil if env.debug
    options[:file] = env.tina_txt_file
    exit unless execute_script "./c/build/petrinet", options, "generate state space...", ''
  else
    exit unless execute_script "#{env.source_path}/generator/generator.rb", options, "generate state space..."
  end
end

# options = {ats: env.ats_json_file, 'full-dot' => env.png ? env.full_dot_file : false,  'reduced-dot' => env.png ? env.reduced_dot_file : false, debug: env.debug, 
# 	   name: (File.basename env.pnml_file, '.*'), o: env.output_file, all: env.check_all, algorithm: env.algorithm}
# exit unless execute_script "#{env.script_path}/por/por.rb", options, 'reduce state space...' unless env.disable_reduction

options = {ats: env.ats_json_file}
exit unless execute_script "./c/build/por_module", options, 'reduce state space...', '' unless env.disable_reduction

if env.png
  puts "generate #{png_source}..."
  system "dot -Tpng #{png_source} -o #{png_file}"
  system "open #{png_file}"
end

