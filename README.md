Prototype of POR using probe set
======================
implementation of por using probe set ( http://wwwhome.ewi.utwente.nl/~rensink/papers/concur2008.pdf )
 
usage
------
### petrinet_por.rb ###
    ruby ./script/petrinet_por --pnml <pnml file> --out <output_file> [options]
    --debug             - Print debug message
    --png               - Draw state space using graphviz
    --disable-reduction - Generate state space without reduction
    --use-cache         - Use existing ats file. ats file is created only if it does not exis
    --use-scala         - Use scala petrinet implementation
### PNML (PetriNet Markup Language) ###
This script generate state space using petrinet model.

The Petri Net Markup Language (PNML) is a proposal of an XML-based interchange format for Petri nets.(http://www.pnml.org/)