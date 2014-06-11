class Vector
  def initialize(state, word)
    @state = state
    @word = word
  end

  def missed_action(actions)
    missed_actions = []

    weak_prefixes = @word.weak_prefix
    actions.each do |a|
      puts a.name
      weak_prefixes.each do |v|
	va = v + a
	wa = @word + a
	puts '=============================================='
	puts "#{va.to_s}: #{@state.enable? va}"
	puts "#{wa.to_s}: #{@state.enable? wa}"
	puts '=============================================='
	puts
	missed_actions.push (a.prime_cause v) + a  if va.feasible? && wa.feasible? && (@state.enable? va) && !(@state.enable? wa)
      end
    end

    missed_actions
  end

  def fresh_missed_action(actions)
  end

  def potentially_missed_action(actions)
  end
end
