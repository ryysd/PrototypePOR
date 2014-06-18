require 'json'

class Dumper
  def self.dump_state_space(state_space)
    state_space.transitions.each do |name, t|
      puts "#{t.src.name}-#{t.action.name}->#{t.dst.name}"
    end
  end

  def self.dump_action_relations(actions)
    actions.each do |a|
      actions.each do |b|
	puts "#{a.name} simulate #{b.name}" if a.simulate? b
	puts "#{a.name} disable #{b.name}" if a.disable? b
      end
    end
  end
end
