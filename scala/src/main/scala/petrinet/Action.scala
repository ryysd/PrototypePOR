package scalamc.petrinet

case class Action(name :String = "", c :Set[String] = Set[String](), d :Set[String] = Set[String](), r :Set[String] = Set[String](), n :Set[String] = Set[String]()) {
  def isSimulate(other :Action) = {
    (d & (other.c | other.n)).nonEmpty
  }

  def isDisable(other :Action) = {
    (d & (other.r | other.d)).nonEmpty
  }
}
