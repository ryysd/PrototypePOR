class AlphabetSerialNumberGenerator
  def self.generate(num)
    alphabets = ('a'..'z').to_a
    length = (num / alphabets.size).to_i
    mod = num % alphabets.size
    
    ('z' * length) + alphabets[mod]
  end
end

class Petrinet2LMNtalConverter
  def convert(petrinet)
    lmntal = []
    lmntal.push make_header petrinet
    lmntal.push make_initial_var_names petrinet.places
    expressions = petrinet.transitions.map do |transition|
      self_loop = calc_self_loop_places transition
      var_table = create_var_table transition, self_loop
      "#{make_rule_name transition} #{make_lhs transition, var_table, self_loop} :- #{make_guard transition, var_table, self_loop} | #{make_rhs transition, var_table, self_loop}"
    end
    lmntal.push expressions

    (lmntal.join ".\n") + ".\n"
  end

  def calc_self_loop_places(transition)
    transition.inputs & transition.outputs
  end

  def create_var_table(transition, self_loop_places)
    places = (transition.inputs + transition.outputs).uniq - self_loop_places
    var_table = places.map.with_index {|p, idx| [p.id, (AlphabetSerialNumberGenerator.generate idx)]}
    loop_var_table = self_loop_places.map.with_index {|p, idx| [p.id, (AlphabetSerialNumberGenerator.generate idx).upcase]}

    (var_table + loop_var_table).to_h
  end

  def make_header(petrinet)
    "// #{petrinet.name}"
  end

  def make_initial_var_names(places)
    places.map.with_index {|place,idx| make_global_var_name place.id, place.initial_marking}.join ','
  end

  def make_local_var_name(val)
    "$#{val}"
  end

  def make_local_tmp_var_name(val)
    "#{make_local_var_name val}_"
  end

  def make_global_var_name(id, val)
    "#{id}(#{val})"
  end

  def make_loop_var_name(place, var_table)
    make_global_var_name place.id, var_table[place.id]
  end

  def make_lhs_var_name(place, var_table)
    make_global_var_name place.id, (make_local_var_name var_table[place.id])
  end

  def make_rhs_var_name(place, var_table)
    make_global_var_name place.id, (make_local_tmp_var_name var_table[place.id])
  end

  def make_inc_expr(place, var_table, val = 1)
    "#{make_local_tmp_var_name var_table[place.id]}=#{make_local_var_name var_table[place.id]}+#{val}"
  end

  def make_dec_expr(place, var_table, val = 1)
    "#{make_local_tmp_var_name var_table[place.id]}=#{make_local_var_name var_table[place.id]}-#{val}"
  end

  def make_rule_name(transition)
    "#{transition.id.gsub(/\W+/, '')} @@"
  end

  def make_lhs(transition, var_table, self_loop_places)
    inputs = transition.inputs - self_loop_places
    outputs = transition.outputs - self_loop_places

    lhs = []
    lhs.push inputs.map {|input| make_lhs_var_name input, var_table}.join ',' unless inputs.empty?
    lhs.push outputs.map {|output| make_lhs_var_name output, var_table}.join ',' unless outputs.empty?
    lhs.push self_loop_places.map {|self_loop| make_loop_var_name self_loop, var_table}.join ',' unless self_loop_places.empty?
    lhs.join ', '
  end

  def make_rhs(transition, var_table, self_loop_places)
    inputs = transition.inputs - self_loop_places
    outputs = transition.outputs - self_loop_places

    rhs = []
    rhs.push inputs.map {|input| make_rhs_var_name input, var_table}.join ',' unless inputs.empty?
    rhs.push outputs.map {|output| make_rhs_var_name output, var_table}.join ',' unless outputs.empty?
    rhs.push self_loop_places.map {|self_loop| make_loop_var_name self_loop, var_table}.join ',' unless self_loop_places.empty?
    rhs.join ', '
  end

  def make_guard(transition, var_table, self_loop_places)
    inputs = transition.inputs - self_loop_places
    outputs = transition.outputs - self_loop_places

    guard = []
    guard.push inputs.map {|input| "#{make_local_var_name var_table[input.id]}>0"}.join ','unless inputs.empty?
    guard.push self_loop_places.map {|self_loop| "#{var_table[self_loop.id]}>0"}.join ','unless self_loop_places.empty?
    guard.push inputs.map {|input| "#{make_dec_expr input, var_table}"}.join ',' unless inputs.empty?
    guard.push outputs.map {|output| "#{make_inc_expr output, var_table}"}.join ',' unless outputs.empty?
    guard.join ', '
  end
end
