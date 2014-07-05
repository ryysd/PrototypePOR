package utgenome

import sbt._
import Keys._
import sbtassembly.Plugin._
import AssemblyKeys._

//assemblySettings

object ProjectBuild extends Build {

  lazy val root = Project(
    id ="petrinet-por",  // Set your project name here
    base = file("."),
    settings = 
    Defaults.defaultSettings 
    ++ assemblySettings
    ++ Seq(PackageTask.packageDistTask) 
    ++ PackageTask.distSettings 
    ++ Seq(
      scalaVersion := "2.9.2",
      //organization := "org.utgenome.sample",
      resolvers += "repo.codahale.com" at "http://repo.codahale.com",
      version := "1.0-SNAPSHOT",
      scalacOptions ++= Seq("-encoding", "UTF-8", "-deprecation", "-unchecked"),
      parallelExecution := true,
      crossPaths := false,
      libraryDependencies ++= Seq(
        "org.codehaus.plexus" % "plexus-classworlds" % "2.4",
        "org.scalatest" %% "scalatest" % "2.0.M1" % "test",
        "com.github.scopt" %% "scopt" % "3.2.0",
        "com.codahale" % "jerkson_2.9.1" % "0.5.0"
      )
  )
   )
}
