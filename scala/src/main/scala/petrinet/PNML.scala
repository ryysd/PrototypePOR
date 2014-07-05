package scalamc.petrinet

import scala.xml.XML
import java.io.File

object PNML {
  def parse(fileName : String, debug :Boolean = false) = {
    val xml = XML.loadFile(new File(fileName))
    val arc_node = xml \\ "arc"

    val getId = (node :scala.xml.Node) => Array((node \ "@id"), (node \\ "id" \\ "text")).find{n => !n.isEmpty}.map{n => n.text}.get
    val getName = (node :scala.xml.Node, default :String) => Array((node \ "@name"), (node \\ "name" \\ "text")).find{p => !p.isEmpty}.map{p => p.text}.getOrElse(default)
    val getInscription = (node :scala.xml.Node) => Array((node \ "@inscription"), (node \\ "inscription" \\ "text")).find{p => !p.isEmpty}.map{p => p.text.toInt}.getOrElse(1)
    val getArcs = (sourceId :String) => arc_node.collect{case n if((n \\ "@source").text == sourceId) => new Arc((n \\ "@source").text, (n \\ "@target").text, getInscription(n))}.toArray
    val getPetrinetNodeInfo = (node :scala.xml.Node) => {
      val id = getId(node)
      val name = getName(node, id)
      val arcs = getArcs(id)
      (id, name, arcs)
    }

    val places = (xml \\ "place").map{place => {
      val (id, name, arcs) = getPetrinetNodeInfo(place)
      val initialMarking = Array((place \\ "initialMarking" \\ "text")).find{p => !p.isEmpty}.map{p => p.text.toInt}.getOrElse(0)
      (id, new Place(id, name,arcs, initialMarking))
    }}.toMap

    val transitions = (xml \\ "transition").map{transition => {
      val (id, name, arcs) = getPetrinetNodeInfo(transition)
      (id, new Transition(id, name, arcs))
    }}.toMap

    new Petrinet(places, transitions, debug)
  }
}


