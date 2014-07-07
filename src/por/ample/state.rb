# State for por using ample set
class State
  def enable?(a, b)
    after(b).enable_actions.include? a
  end

  def commutable?(a, b)
    ((after a).after b) == ((after b).after a)
  end

  def independence?(a, b)
    (enable? a, b) && (commutable? a, b)
  end

  def dependence_actions
    actions = enable_actions
    dependences = []

    actions.each do |a|
      actions.each do |b|
	next if a == b
	ependences.push a, b if !(independence? a, b)
      end
    end

    dependences
  end
end
