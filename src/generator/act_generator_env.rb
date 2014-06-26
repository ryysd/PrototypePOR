require 'optparse'

class ACTGeneratorEnv
  attr_reader :dot_file, :entity_num, :max_action_num, :init_num, :max_state_num, :max_edge_num_per_state, :max_creator_num, :max_reader_num, :max_eraser_num, :max_embargoes_num

  def initialize
    params = ARGV.getopts '', 'dot:', 'state-num:', 'action-num-per-state:', 'max-creator-num:', 'max-eraser-num:'


    @dot_file = params['dot']
    @state_num = params['state-num']

    @action_num_per_state = params['action-num-per-state'] || 3

    @max_creator_num = params['max-creator-num'] || 1
    @max_eraser_num = params['max-eraser-num'] || 0
  end
end
