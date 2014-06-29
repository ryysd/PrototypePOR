require_relative 'generator_env'
require_relative 'ats_file_generator'
require_relative '../petrinet/pnml'

env = GeneratorEnv.new
petrinet = PNML.parse (File.open env.pnml_file).read

result = 
case env.mode
when :ats
  ATSFileGenerator.generate petrinet
when :state_space
  dot = ['digraph {']
  states_num = petrinet.execute do |source, transition, target|
    dot.push "  \"#{source.to_s}\"->\"#{target.to_s}\" [label=\"#{transition.name}\"];"
  end
  Debug.puts_success "number of state: #{states_num}"
  dot.push '}'
  dot.join "\n"
end

File.open env.output_file, 'w' do |file|
  file.write result
end
