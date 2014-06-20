echo 'generate ./tmp/input.act.json...'
ruby ./src/generator/act_file_generator.rb --dot ./tmp/act.dot > ./tmp/input.act.json

echo 'generate ./tmp/input.ats.json...'
ruby ./src/generator/ats_file_generator.rb --act ./tmp/input.act.json --dot ./tmp/ats.dot > ./tmp/input.ats.json

echo 'generate ./tmp/full.dot...'
ruby ./src/por/por.rb --ats './tmp/input.ats.json' --full-dot './tmp/full.dot'

echo 'generate ./tmp/act.png...'
dot -Tpng ./tmp/act.dot -o ./tmp/act.png

echo 'generate ./tmp/ats.png...'
dot -Tpng ./tmp/ats.dot -o ./tmp/ats.png

echo 'generate ./tmp/full.png...'
dot -Tpng ./tmp/full.dot -o ./tmp/full.png

open ./tmp/act.png
open ./tmp/ats.png
open ./tmp/full.png
