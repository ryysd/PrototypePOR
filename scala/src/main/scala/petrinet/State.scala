package scalamc.petrinet

case class State(marking :Array[Int]) {
  def successors(incidenceColumnVectors :Map[String, Array[Int]], transitions :Map[String, Transition]) = {
    transitions.map{case (name, transition) => {
      var valid = true
      val newMarking = (marking.zip(incidenceColumnVectors(name))).map{case (a, b) => {val e = a+b; valid = valid && e >= 0; e}}
      if(valid) Some((transitions(name), new State(newMarking))) else None
    }}.collect{case Some(st) => st}
  }

  override def toString = "[" + marking.mkString(",") + "]"
}

