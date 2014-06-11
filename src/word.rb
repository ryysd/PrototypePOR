class Word
  include Enumerable

  attr_reader :actions

  def initialize(actions)
    @actions = actions
  end

  def influence?(w)
    @actions.each do |a|
      w.each do |b|
	return true if (a.simulate? b) || (b.disable? a)
      end
    end
    false
  end

  def independent?(w)
    !influence w
  end

  # Definition1: word feasibility
  def feasible?
    # for all sub-words a·v·b of w,if a disable b then exist c in Av :a simulate c simulate b;
    @actions.each.with_index do |a, idx_a|
      @actions.drop(idx_a+1).each.with_index do |b, idx_b|
	if a.disable? b
	  v = @actions.slice idx_a, idx_a+idx_b+1
	  return false until v.any?{|c| (a.simulate? c) && (c.simulate? b)}
	end
      end
    end

    # for all sub-words v1・v2 of w, if v2 influence v1 then v1 influence v2;
    (0...length-1).each do |idx|
      head = self[0..idx]
      tail = self[idx+1...length]
      (0...head.length).each do |idx_h|
	(0...tail.length).each do |idx_t|
	  v1 = head[head.length-idx_h-1...head.length]
	  v2 = tail[0..idx_t]
	  return false if (v2.influence? v1) && (!v1.influence? v2)
	end
      end
    end

    true
  end

  #def prime_cause(a)
  #  def _prime_cause_rec(a, actions)
  #    t = actions.pop
  #    v = (t.influence? a) || (actions.any?{|rest| t.influence? rest}) ? t : nil
  #    actions.empty? ? [v] : ((_prime_cause_rec a, actions).push v)
  #  end

  #  Word.new (_prime_cause_rec a, @actions.clone).compact
  #end

  # Definition2: equality up to permutation of independent actions
  # weak equal is the smallest transitive relation such that v·a·b·w ≃ v·b·a·w if a not influence b
  def weak_equal?(w, dump = false)
    return false if length != w.length || self == w
    return self[0] == w[0] if length == 1

    def _remove_duplicate_head(a, b, idx = 0)
      a[idx] == b[idx] ? (_remove_duplicate_head a, b, idx + 1) : [(a.drop idx), (b.drop idx)]
    end

    def _remove_duplicate_tail(a, b, idx = 0)
      a[a.length - idx - 1] == b[b.length - idx - 1] ? (_remove_duplicate_tail a, b, idx + 1) : [(a.take a.length - idx), (b.take b.length - idx)]
    end

    tmp = _remove_duplicate_head self, w
    tmp = _remove_duplicate_tail tmp[0], tmp[1]
    ab = tmp[0]
    ba = tmp[1]

    (0...ab.length).each do |idx|
      length_a = idx + 1
      length_b = ab.length - length_a

      a = ab[0, length_a]
      b = ab[-length_b, length_b]

      return true if (a == ba[-length_a, length_a] && b == ba[0, length_b]) && !((Word.new a).influence? (Word.new b))
    end

    false

    #skip = -1
    #(0...length-1).each do |idx|
    #  next if idx == skip || self[idx] == w[idx]

    #  return false if !(self[idx+1] == w[idx] && self[idx] == w[idx+1] && !(self[idx].influence? self[idx+1]))
    #  skip = idx+1
    #end

    #true
  end

  def hard_prefix
    (0...length).map{|idx| self[0..idx]}
  end

  def weak_prefix
    weak_prefixes = []

    permutation.each do |perm|
      (0...perm.length-1).each do |length|
	weak_prefixes.push perm[0..length]
      end
    end

    weak_prefixes.uniq
  end

  def weak_prefix?(w)
    weak_prefixes.include? w
  end

  def weak_difference(w)
    words = []

    diff = []
    [0...length].each do |idx|
      if self[idx] == w[idx] && !diff.empty? then words.push diff
      else diff.push self[idx]
      end
    end

    words
  end

  # permutation of independent actions
  def permutation
    @actions.permutation.map{|perm| Word.new perm}.select{|w| w.feasible? && (w.weak_equal? self)}
    #permutations = []
    #(0...length-1).each do |idx|
    #  if @actions[idx].independent? @actions[idx+1]
    #    perm = @actions.clone
    #    perm[idx] = @actions[idx+1]
    #    perm[idx+1] = @actions[idx]
    #    permutations.push Word.new perm
    #  end
    #end
    #permutations
  end

  def head(size)
    @actions[0...size]
  end

  def tail(size)
    self[(length-size)...length]
  end

  def length
    @actions.length
  end

  def empty?
    @actions.empty?
  end

  def each
    @actions.each do |action|
      yield action
    end
  end

  def map
    @actions.each do |action|
      yield action
    end
  end

  def ==(w)
    @actions == w.actions
  end

  def hash
    to_s.hash
  end

  def eql?(w)
    hash == w.hash
  end

  def +(w)
    w = Word.new [w] if w.is_a? Action
    Word.new @actions + w.actions
  end

  def [](idx)
    if idx.is_a? Range then Word.new @actions[idx]
    else @actions[idx]
    end
  end

  def to_s
    "[#{@actions.map{|a| a.name}.join ' '}]"
  end
end

