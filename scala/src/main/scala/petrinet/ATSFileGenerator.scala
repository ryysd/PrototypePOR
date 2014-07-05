package scalamc.petrinet

import com.codahale.jerkson.Json._

object ATSFileGenerator {
  def createActions(petrinet :Petrinet) = {
    val inputColumnVectors = petrinet.transitions.zipWithIndex.map{case ((name, _), j) => (name, (0 until petrinet.places.size).map{(i) => petrinet.inputMatrix(i)(j)}.toArray)}.toMap
    val outputColumnVectors = petrinet.transitions.zipWithIndex.map{case ((name, _), j) => (name, (0 until petrinet.places.size).map{(i) => petrinet.outputMatrix(i)(j)}.toArray)}.toMap

    val places = petrinet.places.values.toArray
    val transitions = petrinet.transitions.values.toArray
    (inputColumnVectors.values.zip(outputColumnVectors.values)).zipWithIndex.map{case ((readers, erasers), i) => {
      val reader = readers.zipWithIndex.collect{case (r, j) if(r > 0) => places(j).id}
      val eraser = erasers.zipWithIndex.collect{case (e, j) if(e > 0) => places(j).id}
      new Action(transitions(i).name, reader.toSet, eraser.toSet)
    }}.toArray
  }

  def createActionRelations(actions :Array[Action]) = {
    val relations = new scala.collection.mutable.ArrayBuffer[String]
    actions.foreach{a => {
      actions.foreach{b => {
        if(a isSimulate b) relations += a.name + " s " + b.name
        if(a isDisable b) relations += a.name + " d " + b.name
      }}
    }}

    relations
  }

  def createStateEntities(petrinet :Petrinet, states :Array[State]) = {
    val places = petrinet.places.values.toArray
    states.map{s => (s.toString, s.marking.zipWithIndex.collect{case (m, i) if m > 0 => places(i).id})}.toMap
  }

  def createActionEntities(actions :Array[Action]) = {
    actions.map{a => (a.name, Map("r" -> a.r, "c" -> a.c, "d" -> a.d, "n" -> a.n))}.toMap
  }

  def generate(petrinet :Petrinet, core :Int = 1) = {
    val transitions = new scala.collection.mutable.ArrayBuffer[String] with scala.collection.mutable.SynchronizedBuffer[String]

    val start = System.currentTimeMillis
    val states = petrinet.execute({(source, transition, target) =>{
        transitions += (source.toString + "-" + transition.name + "->" + target.toString)
      }}, core)

    val actions = createActions(petrinet)
    val relations = createActionRelations(actions)
    val stateEntities = createStateEntities(petrinet, states.values.toArray)
    val actionEntities = createActionEntities(actions)

    println("----------------------------------------------")
    println("number of states: " + states.size)
    println("execution time: " + (System.currentTimeMillis - start) + "ms")
    println("----------------------------------------------")

    val json = Map("actions" -> Map("relations" -> relations, "entities" -> actionEntities), "lts" -> Map("init" -> petrinet.initState.toString, "transitions" -> transitions,  "states" -> stateEntities))
    com.codahale.jerkson.Json.generate(json)
  }
}
