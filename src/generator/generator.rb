require_relative 'generator_env'
require_relative 'ats_file_generator'
require_relative '../petrinet/pnml'

env = GeneratorEnv.new

t_start = Time.now.instance_eval { self.to_i * 1000 + (usec/1000) }
petrinet = PNML.parse (File.open env.pnml_file).read
t_end = Time.now.instance_eval { self.to_i * 1000 + (usec/1000) }

Debug.puts_success "parse time: #{t_end - t_start} ms"

result = 
case env.mode
when :ats
  ATSFileGenerator.generate petrinet
when :state_space
  dot = ['digraph {']
  t_start = Time.now.instance_eval { self.to_i * 1000 + (usec/1000) }
  states_num = petrinet.execute do |source, transition, target|
    dot.push "  \"#{source.to_s}\"->\"#{target.to_s}\" [label=\"#{transition.name}\"];"
  end
  t_end = Time.now.instance_eval { self.to_i * 1000 + (usec/1000) }
  Debug.puts_success "execute time: #{t_end - t_start} ms"
  Debug.puts_success "number of state: #{states_num}"
  dot.push '}'
  dot.join "\n"
end

File.open env.output_file, 'w' do |file|
  file.write result
end
