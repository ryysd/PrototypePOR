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
    --use-scala         - Use petrinet of scala implementation
    --all               - Continue reduction, even if a deadlock state is found

usage (for LMNtal)
------
### petrinet -> lmntal -> ats -> probe por
` script/por_via_slim.rb --slim <slim_file> --pnml <pnml_file>`

### petrinet -> lmntal -> critical por
`ruby script/por_using_slim.rb --slim <slim_file> --pnml <pnml_file>`

### petrinet -> probe por
`ruby script/petrinet_por.rb --pnml <pnml_file>`

PNML (PetriNet Markup Language)
------
This script generate state space using petrinet model.

The Petri Net Markup Language (PNML) is a proposal of an XML-based interchange format for Petri nets.(http://www.pnml.org/)
