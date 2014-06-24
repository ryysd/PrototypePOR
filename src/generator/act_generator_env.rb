require 'optparse'

class ACTGeneratorEnv
  attr_reader :dot_file, :entity_num, :max_action_num, :init_num, :max_state_num, :max_edge_num_per_state, :max_creator_num, :max_reader_num, :max_eraser_num, :max_embargoes_num

  def initialize
    params = ARGV.getopts '', 'dot:', 'entity-num:', 'max-action-num:', 'init-num:', 'max-state-num:', 'max-edge-num-per-state:', 'max-creator-num:', 'max-reader-num:', 'max-eraser-num:', 'max-embargoes-num:'

    default_set_num = 1024

    @dot_file = params['dot']
    @entity_num = params['entity-num']
    @max_action_num = params['max-action-num']
    @max_state_num = params['max-state-num']

    @init_num = params['init-num'] || 0
    @max_edge_num_per_state = params['max-edge-num-per-state'] || 3

    @max_creator_num = params['max-creator-num'] || default_set_num
    @max_reader_num = params['max-reader-num'] || default_set_num
    @max_eraser_num = params['max-eraser-num'] || default_set_num
    @max_embargoes_num = params['max-embargoes-num'] || default_set_num
  end
end
