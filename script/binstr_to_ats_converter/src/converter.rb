require 'pp'
require 'json'

module EditType
  NONE = 0
  INSERT = 1
  REMOVE = 2
  REPLACE = 3
end

class EditLog
  attr_reader :source, :target, :type

  def initialize(source, target, type)
    @source = source
    @target = target
    @type = type
  end
end

class Levenshtein
  def self.create_cost_table(array1, array2)
    table = (Array.new array1.size + 1).map{Array.new array2.size + 1}
    (0..array1.size).each{|i| table[i][0] = i}
    (0..array2.size).each{|i| table[0][i] = i}

    (1..array1.size).each do |i|
      (1..array2.size).each do |j|
        cost = if array1[i] == array2[j] then 0 else 1 end
        table[i][j] = [table[i - 1][j] + 1, table[i][j - 1] + 1, table[i - 1][j - 1] + cost].min
      end
    end

    table
  end

  def self.calc_edit_sequence(array1, array2)
    table = create_cost_table array1, array2
    (row, col) = [array1.size, array2.size]

    sequence = []
    while(row > 0 && col > 0)
      type = [[table[row - 1][col], EditType::INSERT] , [table[row][col - 1], EditType::REMOVE], [table[row - 1][col - 1], EditType::REPLACE]].min_by{|c| c.first}.last

      (row, col) = case type
                when EditType::INSERT then [row - 1, col]
                when EditType::REMOVE then [row, col - 1]
                when EditType::REPLACE then [row - 1, col - 1]
                end

      type = EditType::NONE if type == EditType::REPLACE && array1[row - 1] == array2[col - 1]
      sequence.push EditLog.new row - 1, col - 1, type unless type == EditType::NONE
    end

    sequence
  end
end

class Action
  attr_reader :name, :creator, :eraser, :reader, :embargoes

  def initialize(name, creator, eraser, reader, embargoes)
    @name = name
    @creator = creator.sort
    @eraser = eraser.sort
    @reader = reader.sort
    @embargoes = embargoes.sort
  end

  def to_s
    "c:#{@creator.join}_d:#{@eraser.join}_r:#{@reader.join}_n:#{@embargoes.join}"
  end

  def to_hash
    {@name => {c: @creator, d: @eraser, r: @reader, n: @embargoes}}
  end
end

class BinStr2ATSConverter
  def convert(data)
    @action_names = {}

    # actions = []
    init_state = []
    entities = {}

    data.lines.each.with_index do |line, idx|
      arr = line.chomp.split '->'
      if arr.size == 1
        init_state = arr[0].strip.split ' '
        next
      end

      array1 = arr[0].strip.split ' '
      rule_name = arr[1].strip
      array2 = arr[2].strip.split ' '

      sequence = Levenshtein.calc_edit_sequence array1, array2
      action_entities = sequence.map{|l| edit_log_to_action array1, array2, l, idx}
      action_hash = action_entities.inject{|a, e| a.keys.each{|key| a[key] += e[key]}; a}
      action_sorted_hash =  Hash[action_hash.map { |k,v| [k, v.sort] }]
      action_name = make_action_name rule_name, action_sorted_hash
      entities[action_name] = action_sorted_hash
      # actions.push Action.new action_name, action_sorted_hash[:c], action_sorted_hash[:d], action_sorted_hash[:r], action_sorted_hash[:n]
    end

    # actions.uniq{|a| a.to_s}
    init_entities = make_init_entities init_state
    # action_entities = actions.map {|a| a.to_hash}

    {lts: {init_entities: init_entities}, actions: {entities: entities}}
  end

  def make_init_entities(init_state)
    init_state.map.with_index {|e, i| "#{i}_#{e}"}
  end

  def edit_log_to_action(source, target, log, line)
    case log.type
    when EditType::INSERT then {c: ["#{log.source+1}_#{target[log.target]}"], d: [], r: [], n: []} # TODO: 末端以外への追加は認めないように変更
    when EditType::REMOVE then raise "#{line}: remove element is unsupported." #{c: [], e: [source[log.source]], r: [], n: []}
    when EditType::REPLACE then {c: ["#{log.source}_#{target[log.target]}"], d: ["#{log.source}_#{source[log.source]}"], r: [], n: []}
    end
  end

  def make_action_name(rule_name, entities)
    unless @action_names.has_key? rule_name
      @action_names[rule_name] = []
      @action_names[rule_name].push entities
    end

    duplications = @action_names[rule_name]
    duplications.map.with_index do |e, idx|
      return "#{rule_name}:#{idx + 1}" if e == entities
    end

    duplications.push entities
    return "#{rule_name}:#{duplications.size}"
  end
end

converter = BinStr2ATSConverter.new
json = converter.convert (File.open './tmp/binstr').read
puts JSON.generate json
