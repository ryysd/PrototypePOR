class Vector
  def initialize(state, word)
    @state = state
    @word = word
  end

  def after
    @state.after @word
  end

  def missed_action(actions)
    missed_actions = []

    weak_prefixes = @word.weak_prefix
    actions.each do |a|
      # avoid x0x1y1 x0
      unless @word.include? a
	puts a.name
	weak_prefixes.each do |v|
	  va = v + a
	  wa = @word + a
	  puts '=============================================='
	  puts "#{va.to_s}: #{@state.enable? va}"
	  puts "#{wa.to_s}: #{@state.enable? wa}"
	  puts '=============================================='
	  puts
	  missed_actions.push (a.prime_cause v) + a  if (@state.enable? va) && !(@state.enable? wa)
	end
      end
    end

    missed_actions.uniq
  end

  def fresh_missed_action(actions)
  end

  def potentially_missed_action(actions)
  end
end
