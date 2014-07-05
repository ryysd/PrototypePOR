package scalamc.petrinet

import scala.actors.Actor
import scala.actors.Exit
import scala.actors.Actor._
import scala.collection.JavaConversions._ 
import java.io.File
import java.io.PrintWriter

case class Arc(sourceId :String, targetId :String, inscription :Int) {}
class PetrinetNode(val id :String, val name :String, val arcs :Array[Arc]) { def findArc(targetId :String) :Option[Arc] = arcs.find{a => a.targetId == targetId} }
class Place(id :String, name :String, arcs :Array[Arc], val initialMarking :Int) extends PetrinetNode(id, name, arcs) {}
class Transition(id :String, name :String, arcs :Array[Arc]) extends PetrinetNode(id, name, arcs) {}

case class Petrinet(places :Map[String, Place], transitions :Map[String, Transition], debug :Boolean = false) {
  val initState = new State(places.map{case (_, p) => p.initialMarking}.toArray)
  val (incidenceMatrix, inputMatrix, outputMatrix) = createMatrix()

  def createMatrix() = {
    val rowLen = places.size
    val colLen = transitions.size

    val input_matrix_arr = Array.ofDim[Int](rowLen, colLen)
    val output_matrix_arr = Array.ofDim[Int](rowLen, colLen)
    val incidence_matrix_arr = Array.ofDim[Int](rowLen, colLen)

    places.zipWithIndex.foreach{case ((place_name, place), i) => {
      transitions.zipWithIndex.foreach{case ((transition_name, transition), j) => {
        input_matrix_arr(i)(j) = transition.findArc(place.id).map{arc => arc.inscription}.getOrElse(0)
        output_matrix_arr(i)(j) = place.findArc(transition.id).map{arc => arc.inscription}.getOrElse(0)
        incidence_matrix_arr(i)(j) = input_matrix_arr(i)(j) - output_matrix_arr(i)(j)
      }}
    }}

    val colLabel = transitions.map{case (name, transition) => transition.name}.toArray
    val rowLabel = places.map{case (name, place) => place.name}.toArray

    (incidence_matrix_arr, input_matrix_arr, output_matrix_arr)
  }

  def parallelExecute(callback :(State, Transition, State) => Unit, core :Int) = {
    val incidenceColumVectors = transitions.zipWithIndex.map{case ((name, _), j) => (name, (0 until places.size).map{(i) => incidenceMatrix(i)(j)}.toArray)}.toMap

    val states = new java.util.concurrent.ConcurrentHashMap[String, State]
    val stack = new scala.collection.mutable.SynchronizedStack[State]

    println("number of actors = " + core)

    val workers = (0 until core).map {i =>{ 
      val worker =
      actor {
        loop {
          react {
            case Exit => exit
            case state :State => {
              if(states.putIfAbsent(state.toString, state) == null) {
                  if(debug && states.size % 10000 == 0) println(states.size + "states are expanded")
                  state.successors(incidenceColumVectors, transitions).foreach{case (transition, successor) => {
                  callback(state, transition, successor)
                  stack.push(successor)
                }}
              }
            }
          }
        }
      }
      worker.start()
      worker
    }}

    stack.push(initState)
    while(!(stack.isEmpty && workers.forall{worker => worker.getState == Actor.State.Suspended})) {
      while(stack.nonEmpty) {
        workers(states.size % core) ! stack.pop
      }
    }
    workers.foreach{worker => worker ! Exit}

    states.toMap
  }

  def sequentialExecute(callback :(State, Transition, State) => Unit) = {
    val incidenceColumVectors = transitions.zipWithIndex.map{case ((name, _), j) => (name, (0 until places.size).map{(i) => incidenceMatrix(i)(j)}.toArray)}.toMap

    val states = new scala.collection.mutable.HashMap[String, State]
    val stack = new scala.collection.mutable.Stack[State]

    stack.push(initState)
    while(!stack.isEmpty) {
      val state = stack.pop

      if(!states.contains(state.toString)) {
        states += (state.toString -> state)
        state.successors(incidenceColumVectors, transitions).foreach{case (transition, successor) => {
          if(debug && states.size % 10000 == 0) println(states.size + "states are expanded")
          callback(state, transition, successor)
          stack.push(successor)
        }}
      }
    }

    states
  }

  def execute(callback :(State, Transition, State) => Unit, core :Int = 1) = {
    if(core == 1) sequentialExecute(callback)
    else parallelExecute(callback, core)
  }
}

case class Config(pnml :String = "", out :String = "", debug :Boolean = false, ats :Boolean = true, stateSpace :Boolean = false, core :Int = 1)

object Petrinet {
  def main(args:Array[String]) = {

    val parser = new scopt.OptionParser[Config]("scopt") {
      head("scopt", "3.x")
      opt[String]('p', "pnml") required() valueName("<pnml-file>") action { (x, c) =>
        c.copy(pnml = x) } text("pnml is a file name of pnml")
      opt[String]('o', "out") required() valueName("<output-file>") action { (x, c) =>
        c.copy(out = x) } text("out is a file name of output file")
      opt[Int]('w', "worker-num") required() valueName("<worker-num>") action { (x, c) =>
        c.copy(core = x) } text("worker-num is a number of workers")
      opt[Unit]("state-space") hidden() action { (_, c) =>
        c.copy(stateSpace = true) } text("this option is hidden in the usage text")
      opt[Unit]("debug") hidden() action { (_, c) =>
        c.copy(debug = true) } text("this option is hidden in the usage text")
      opt[Unit]("ats") hidden() action { (_, c) =>
        c.copy(ats = true) } text("")
      help("help") text("prints this usage text")
    }

    parser.parse(args, Config()) map { config =>
      val petrinet = PNML.parse(config.pnml, config.debug)
      val json = ATSFileGenerator.generate(petrinet, config.core)
      val out = new PrintWriter(config.out)
      out.println(json)
      out.close
    } getOrElse {
      // arguments are bad, error message will have been displayed
    }
  }
}
