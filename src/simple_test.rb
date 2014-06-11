require 'pp'
require_relative 'reducer'
require_relative 'util/simulation_disabling_file_parser'
require_relative 'util/transition_file_parser'
require_relative 'util/colored_string'
require_relative 'util/test_printer'

#relations = {
#  x1: {x1: :d , x2: :s , x0: nil, y1: nil, y2: nil, y0: :d},
#  x2: {x1: nil, x2: :d , x0: :s , y1: nil, y2: :d , y0: nil},
#  x0: {x1: :s , x2: nil, x0: :d , y1: nil, y2: :s , y0: :s},
#  y1: {x1: nil, x2: nil, x0: :d , y1: :d , y2: :s , y0: nil},
#  y2: {x1: nil, x2: :d , x0: nil, y1: nil, y2: :d , y0: :s},
#  y0: {x1: nil, x2: :s , x0: :s , y1: :s , y2: nil, y0: :d}
#}
#
#def mk_actions(relations)
#  actions = ActionTable.new
#
#  relations.each{|k, v| actions.create k}
#  relations.each do |k, v|
#    src = actions[k]
#
#    v.each do |nk, nv|
#      dst = actions[nk]
#      case nv
#      when :d then src.disable dst
#      when :s then src.simulate dst
#      when nil
#      end
#    end
#  end
#
#  actions
#end
#actions = mk_actions relations
#

class DebugPrinter
end

actions = SimulationDisablingFileParser.parse './input/sample.sd'
actions.dump
x0   = Word.new [actions[:x0]]
x1   = Word.new [actions[:x1]]
x2   = Word.new [actions[:x2]]
y0   = Word.new [actions[:y0]]
y1   = Word.new [actions[:y1]]
y2   = Word.new [actions[:y2]]
x1y1 = x1 + y1
x1x2 = x1 + x2
y1y2 = y1 + y2
x1x2y1 = x1x2 + y1
x1y1x2 = x1y1 + x2
x2x1y1 = x2 + x1 + y1
x1x0x2 = x1 + x0 + x2
x1x2x0 = x1 + x2 + x0
x2x1 = x2 + x1
y2y1 = y2 + y1
x1x2x0y2y1 = x1+x2+x0+y2+y1

x1y1x2.weak_prefix.each{|w| puts w.to_s}

# おかしい
# x1x0x2がfeasibleなのもおかしいが、互いにfeasibleなwordに関して、weak equalがsynmetricじゃないのもおかしい
# 追記: 修正した
# TestPrinter.print_result x1x0x2.feasible?
# TestPrinter.print_result x1x2x0.feasible?
# TestPrinter.print_result x1x0x2.weak_equal? x1x2x0
# TestPrinter.print_result x1x2x0.weak_equal? x1x0x2

TestPrinter.print_result x1x2x0y2y1.feasible?


all_actions = [:x1, :x2, :x0, :y1, :y2, :y0]
all_words = []
(1..all_actions.length).each do |idx|
  sub_word = all_actions[0...idx]
  sub_word.permutation.each{|perm| all_words.push Word.new perm.map{|p| actions[p]}}
end
feasible_words = all_words.select{|w| w.feasible?}

unique_feasible_words = []
work = feasible_words.clone

until work.empty?
  target = work.shift
  work.reject!{|w| target.weak_equal? w}
  unique_feasible_words.push target
end

# word test start
TestPrinter.test_start 'word test'

TestPrinter.test_case_start 'number of all feasible words:'
puts feasible_words.length
TestPrinter.test_case_end

TestPrinter.test_case_start 'all feasible words:'
feasible_words.each{|w| w.actions.each{|a| print a.name}; puts}
TestPrinter.test_case_end

TestPrinter.test_case_start 'number of unique feasible words:'
puts unique_feasible_words.length
TestPrinter.test_case_end

TestPrinter.test_case_start 'unique feasible words:'
unique_feasible_words.each{|w| w.actions.each{|a| print a.name}; puts}
TestPrinter.test_case_end

TestPrinter.test_case_start 'all unique feasible words are weak equal each other:'
TestPrinter.print_result feasible_words.permutation(2).all?{|a,b| 
  if (a.weak_equal? b)
    print "#{a.actions.map{|act| act.name}} weak_equal? #{b.actions.map{|act| act.name}}: " 
    TestPrinter.print_result a.weak_equal? b
    print "#{b.actions.map{|act| act.name}} weak_equal? #{a.actions.map{|act| act.name}}: "
    TestPrinter.print_result b.weak_equal? a
  end
  result = (a.weak_equal? b) == (b.weak_equal? a)
  result
}
TestPrinter.test_case_end

TestPrinter.test_end
# word test end


# permutation test start
TestPrinter.test_start 'permutation test'

TestPrinter.test_case_start 'permutation of x1y1x2:'
x1y1x2.permutation.each{|perm| p perm.actions.map{|v| v.name}}
TestPrinter.test_case_end

TestPrinter.test_case_start 'all permutations weak equal origin:'
TestPrinter.print_result x1y1x2.permutation.all?{|perm| perm.weak_equal? x1y1x2}
TestPrinter.test_case_end

TestPrinter.test_case_start 'all permutations is feasible:'
TestPrinter.print_result x1y1x2.permutation.all?{|perm| perm.feasible?}
TestPrinter.test_case_end

TestPrinter.test_end
# permutation test end

puts
puts x1x2y1.feasible?
puts x2x1y1.feasible?

puts x1x2.influence? y1y2
puts x1.influence? y1y2

puts x1x2y1.weak_equal? x1y1x2
puts x1x2y1.weak_equal? x2x1y1
p x2.influence? x1

pp (x1y1.prime_cause actions[:y2]).map{|p| p.name}
pp (x1.prime_cause actions[:x2]).map{|p| p.name}
pp (x1x2.prime_cause actions[:x0]).map{|p| p.name}
pp (x1x2.prime_cause actions[:y1]).map{|p| p.name}
pp (x1.prime_cause actions[:y2]).map{|p| p.name}

#action_table = SimulationDisablingFileParser.parse './input/sample.sd'
ss = TransitonFileParser.parse './input/sample.tr', actions
ss.dump_dot
