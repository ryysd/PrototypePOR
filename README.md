Prototype of POR using probe set
======================
implementation of por using probe set ( http://wwwhome.ewi.utwente.nl/~rensink/papers/concur2008.pdf )
 
usage
------
### por.rb ###
    ruby ./src/por.rb --ats [ats_file]

por.rb generates two dot files from ATS file.

1. ss.dot: full state space
2. reduced_ss.dot: reduced state space

### ats_file_generator.rb ###
    ruby ./src/entity_based_transition_system/generator/ats_file_generator.rb --act [act_file]
ats_file_generator.rb generates ATS file from ACT file.

### act_file_generator.rb ###
    ruby ./src/entity_based_transition_system/generator/act_file_generator.rb
entity_generator.rb generates ACT file.

file format
------
### ats file ###
json format file of Actions and Transition System

    {    
      "actions": {
      "relations": [
        "x1 s x2",
        …
      ]
    },
    "lts": {
      "init": "s",
      "transitions": [
          "s - x1 -> q1",
          ...
        ]
      }
    }

### act file ###
json format file of Actions using entity

    {
      "init": [],
      "actions": {
        "x1": {
          "r": [],
          "n": ["ex1"],
          "d": [],
          "c": ["ex0"]
        },
        "x2": {
          ...
        },
        ...
      }
    }