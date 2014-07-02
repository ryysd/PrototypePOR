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
	weak_prefixes.each do |v|
	  va = v + a
	  wa = @word + a
	  missed_actions.push (a.prime_cause v) + a  if (@state.enable? va) && !(@state.enable? wa)
	end
      end
    end

    missed_actions.uniq
  end

  def fresh_missed_action(actions, detected_missed_actions)
    missed_actions = potentially_missed_action actions 

    hard_prefixes = (0...@word.length-1).map{|i| @word[0..i]}
    detected_missed_actions[to_s] = missed_actions

    hard_prefixes.each do |prefix|
      prefix_vector = Vector.new @state, prefix
      prefix_missed_actions = detected_missed_actions[prefix_vector.to_s]
      if prefix_missed_actions.nil?
	prefix_missed_actions = prefix_vector.missed_action @actions
	detected_missed_actions[prefix_vector.to_s] = prefix_missed_actions
      end

      prefix_missed_actions_last = prefix_missed_actions.map{|ma| ma.last}.uniq
      missed_actions.reject! do |ma|
	a = ma.last
	prefix_missed_actions_last.include? a
      end
    end

    missed_actions
  end

  # Definition 17: potentially missed actions
  # (q, w・b) be a vector. Then, a is a potentially missed action if either b disable a or the following conditions hold
  # 1: a is weakly but not strongly enabled
  # 2: a was somewhere disabled
  # 3: a is freshly enabled
  def potentially_missed?(a, b)
    condition_1 = (@state.weak_enable? @word, a) && !(after.enable? a)
    condition_2 = @word.any?{|c| c.disable? a}
    condition_3 = b.simulate? a

    (b.disable? a) || (condition_1 && condition_2 && condition_3)
  end

  def potentially_missed_action(actions)
    b = @word.last
    return [] if b.nil?

    actions.select{|a| potentially_missed? a, b}.map{|a| (a.prime_cause @word) + a}.select{|a| @state.enable? a}
  end

  def to_s
    "#{@state.name}:#{@word.to_s}"
  end
end
