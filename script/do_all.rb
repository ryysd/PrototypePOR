require 'optparse'
require_relative '../src/util/debug'

class ScriptEnv
  attr_reader :debug, :png, :png_all
  attr_reader :state_num, :action_num_per_state, :max_creator_num, :max_eraser_num

  def initialize
    params = ARGV.getopts '', 'debug', 'png', 'png-all', 'state-num:', 'action-num-per-state:', 'max-creator-num:', 'max-eraser-num:'
    @debug = params['debug']
    @png = params['png'] || params['png-all']
    @png_all = params['png-all']

    @state_num = (params['state-num'] || 100).to_i
    @action_num_per_state = (params['action-num-per-state'] || 3).to_i
    @max_creator_num = (params['max-creator-num'] || 1).to_i
    @max_eraser_num = (params['max-eraser-num'] || 0).to_i
  end

  tmp_dir = './tmp'
  ['act', 'ats', 'full', 'reduced'].each do |name|
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

def execute_script(script, options, gen_file, stdout = nil)
  puts "generate #{gen_file}"
  cmd = "ruby #{script} #{(Option.new options).to_s}"
  cmd += " > #{stdout}" unless stdout.nil?

  Debug.puts_success cmd
  puts
  system cmd
end


options = {dot: env.act_dot_file, 'state-num' => env.state_num, 'action-num-per-state' => env.action_num_per_state, 'max-creator-num' => env.max_creator_num, 'max-eraser-num' => env.max_eraser_num}
execute_script './src/generator/act_file_generator.rb', options, env.act_json_file, env.act_json_file

options = {act: env.act_json_file, dot: env.ats_dot_file}
execute_script './src/generator/ats_file_generator.rb', options, env.ats_json_file, env.ats_json_file

options = {ats: env.ats_json_file, 'full-dot' => env.full_dot_file,  'reduced-dot' => env.reduced_dot_file, debug: env.debug}
execute_script './src/por/por.rb', options, env.reduced_dot_file

if env.png
  if env.png_all
    puts "generate #{env.act_png_file}..."
    system "dot -Tpng #{env.act_dot_file} -o #{env.act_png_file}"

    puts "generate #{env.ats_png_file}..."
    system "dot -Tpng #{env.ats_dot_file} -o #{env.ats_png_file}"

    puts "generate #{env.full_png_file}..."
    system "dot -Tpng #{env.full_dot_file} -o #{env.full_png_file}"

    system "open #{env.act_png_file}"
    system "open #{env.ats_png_file}"
    system "open #{env.full_png_file}"
  end

  puts "generate #{env.reduced_png_file}..."
  system "dot -Tpng #{env.reduced_dot_file} -o #{env.reduced_png_file}"
  system "open #{env.reduced_png_file}"
end
