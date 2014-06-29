Prototype of POR using probe set
======================
implementation of por using probe set ( http://wwwhome.ewi.utwente.nl/~rensink/papers/concur2008.pdf )
 
usage
------
### petrinet_por.rb ###
    ruby ./script/petrinet_por --pnml \<pnml file> [--debug] [--png]
    --debug - Print debug message
    --png   - draw state space using graphviz
### PNML (PetriNet Markup Language) ###
This script generate state space using petrinet model.

The Petri Net Markup Language (PNML) is a proposal of an XML-based interchange format for Petri nets.(http://www.pnml.org/)