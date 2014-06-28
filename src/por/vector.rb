class Vector
  attr_reader :state, :word

  def initialize(state, word)
    @state = state
    @word = word
  end

  def after
    @state.after @word
  end

  def missed_action(actions)
    missed_actions = []

    Debug.dprint "calc weak prefix: #{@word.to_s}...   "
    t_start = Time.now.instance_eval { self.to_i * 1000 + (usec/1000) }

    weak_prefixes = @word.weak_prefix

    t_end = Time.now.instance_eval { self.to_i * 1000 + (usec/1000) }
    Debug.dputs "#{(t_end - t_start)} ms. [size: #{weak_prefixes.length}]"

    actions.each do |a|
      unless @word.include? a
	#puts "  #{a.name}"
	weak_prefixes.each do |v|
	  va = v + a
	  wa = @word + a
	  #puts '  =============================================='
	  #print "  #{va.to_s}: "; Debug.puts_boolean @state.enable? va
	  #print "  #{wa.to_s}: "; Debug.puts_boolean @state.enable? wa
	  #puts "  prime cause of #{a.name}|#{v.to_s}: #{(a.prime_cause v).to_s}"
	  #puts '  =============================================='
	  #puts
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
