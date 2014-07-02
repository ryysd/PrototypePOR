require_relative 'generator_env'
require_relative 'ats_file_generator'
require_relative '../petrinet/pnml'
require_relative '../util/simple_timer'

timer = SimpleTimer.new
env = GeneratorEnv.new

timer.start
petrinet = PNML.parse (File.open env.pnml_file).read
Debug.dputs "parse time: #{timer.stop} ms"

timer.start
result = 
  case env.mode
  when :ats
    ATSFileGenerator.generate petrinet
  when :state_space
    dot = ['digraph {']
    states = petrinet.execute do |source, transition, target|
      dot.push "  \"#{source.to_s}\"->\"#{target.to_s}\" [label=\"#{transition.name}\"];"
    end
    Debug.puts_success "number of states: #{states.length}"
    dot.push '}'
    dot.join "\n"
  end
Debug.dputs "execution time: #{timer.stop} ms"


File.open env.output_file, 'w' do |file|
  file.write result
end
