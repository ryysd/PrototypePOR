class Reducer
  def ample_condition_1(state, ample_candidate)
    work_queue = [state]

    visited = []
    until work_queue.empty?
      state = work_queue.pop

      next if visited.include? state.name
      visited.push state.name

      unample_set = state.enable_actions - ample_candidate
      next if unample_set.empty?

      return false if unample_set.any?{|a| ample_candidate.any?{|b| !state.independence? a, b}}
      unample_set.each{|a| work_queue.push state.after a}
    end

    true
  end

  def ample_set(s)
    return [] if s.enable_actions.empty?
    return s.enable_actions if s.enable_actions.length == 1
    
    dependence_actions = s.dependence_actions
    ample_candidate = dependence_actions.empty? ? [s.enable_actions.first] : dependence_actions

    (ample_condition_1 s, ample_candidate) ? ample_candidate : s.enable_actions
  end

  def reduce(check_all = true)
    pre_reduce

    work_queue = [@states.init]

    until work_queue.empty?
      state = work_queue.pop
      next if visited? state

      visit state
      break if !check_all && state.deadlock?

      (ample_set state).each{|a| work_queue.push state.after a}
    end

    @states
  end
end
