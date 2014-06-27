require 'optparse'

class ACTGeneratorEnv
  attr_reader :dot_file, :state_num, :action_num_per_state, :max_creator_num, :max_eraser_num

  def initialize
    params = ARGV.getopts '', 'dot:', 'state-num:', 'action-num-per-state:', 'max-creator-num:', 'max-eraser-num:'


    @dot_file = params['dot']

    @state_num = params['state-num'].to_i
    @action_num_per_state = params['action-num-per-state'].to_i || 3

    @max_creator_num = params['max-creator-num'].to_i || 1
    @max_eraser_num = params['max-eraser-num'].to_i || 0
  end
end
