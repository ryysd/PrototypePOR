require 'json'
require_relative 'debug'

class ATSFileReader
  def self.create_action_table(data)
    action_table = ActionTable.new
    entities = data['entities'].nil? ? {} : data['entities']

    Debug.puts_error 'actions => entities key is missing.' if data['entities'].nil?
    data['order'].each{|s| s_entities = entities[s].nil? ? {} : entities[s]; action_table.create s.to_sym, s_entities['c'], s_entities['r'], s_entities['d'], s_entities['m']} unless data['order'].nil?
    data['relations'].each do |rel|
      case rel.chomp.strip
      when /(\w+)\s+(s|d)\s+(\w+)/ 
        l_entities = entities[$1].nil? ? {} : entities[$1]
        r_entities = entities[$3].nil? ? {} : entities[$1]
	l = action_table.create $1.to_sym, l_entities['c'], l_entities['r'], l_entities['d'], l_entities['m']
	r = action_table.create $3.to_sym, r_entities['c'], r_entities['r'], r_entities['d'], r_entities['m']
	$2 == 's' ? (l.simulate r) : (l.disable r)
      end
    end

    action_table
  end

  def self.create_state_space(data, action_table)
    state_space = StateSpace.new
    entities = data['states'].nil? ? {} : data['states']

    state_space.create_as_init data['init'].to_sym, entities[data['init']]

    data['transitions'].each do |trans|
      case trans.chomp.strip
      when /(\w+)\s*-\s*(\w+)\s*->\s*(\w+)/ 
	l = state_space.create $1.to_sym, entities[$1]
	r = state_space.create $3.to_sym, entities[$3]
	action = action_table.create $2.to_sym
	l[action] = r
      end
    end

    state_space
  end

  def self.read(file_name)
    json = JSON.load File.read file_name
    action_table = ATSFileReader.create_action_table json['actions']
    state_space = ATSFileReader.create_state_space json['lts'], action_table

    {action_table: action_table, state_space: state_space}
  end
end
