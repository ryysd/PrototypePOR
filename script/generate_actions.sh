ruby ./src/generator/act_file_generator.rb --dot ./tmp/act.dot
dot -Tpng -o ./tmp/act.png ./tmp/act.dot
open ./tmp/act.png
