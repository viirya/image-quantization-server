
package org.viirya.quantizer

import scala.io.Source


case class MTree[T](id: Int, value: Array[T], var children: List[MTree[T]]) {
    def this(id: Int, value: Array[T]) = this(id, value, List())
    override def toString = "M(" + value.toString + " {" + children.map(_.toString).mkString(",") + "})"
}

/*
object MTree {
  def apply[T](id: Int, value: Array[T]) = new MTree(id, value, List())
  def apply[T](id: Int, value: Array[T], children: List[MTree[T]]) = new MTree(id, value, children)
}
*/

class TreeDataLoader(filepath: String) {
    var line_counter = 0
    var tree_degree_per_node = 0
    var tree_height = 0

    var num_all_tree_nodes = 0
    var num_not_leaf_nodes = 0

    var feature_dimension = 128

    var tree: List[MTree[Float]] = List()
    var root = new MTree(0, new Array[Float](128))

    tree = tree ::: List(root)

    var current_pos_in_tree = 0
    var current_parent_node_in_tree = 0

    Source.fromFile(filepath)
        .getLines
        .foreach { read_line =>
            var line = read_line.stripLineEnd
            var split_line: List[String] = List.fromString(line, ' ')
            line_counter match {
                case 0 =>
                    try {
                        tree_degree_per_node = split_line(0).toInt
                        tree_height = split_line(1).toInt

                        num_all_tree_nodes = ((Math.pow(tree_degree_per_node, tree_height + 1) - 1) / (tree_degree_per_node - 1)).toInt - 1
                        num_not_leaf_nodes = num_all_tree_nodes - Math.pow(tree_degree_per_node, tree_height).toInt
                    } catch {
                        case _: java.lang.NumberFormatException => println("Error file format.")
                    }
                case _ =>
                    var node_value: Array[Float] = split_line.map( (s) => s.toFloat ).toArray
                    var new_node: MTree[Float] = null 

                    if (line_counter <= num_not_leaf_nodes) 
                        new_node = new MTree(-line_counter, node_value)
                    else
                        new_node = new MTree(line_counter - num_not_leaf_nodes, node_value)

                    tree(current_parent_node_in_tree).children = tree(current_parent_node_in_tree).children ::: List(new_node)
                    tree = tree ::: List(new_node)

                    current_pos_in_tree = current_pos_in_tree + 1
                    if (line_counter % tree_degree_per_node == 0)
                        current_parent_node_in_tree = current_parent_node_in_tree + 1
            }
            //println("Processing line " + line_counter)
            line_counter += 1
        }
}

object Quantizer {

    def main(args: Array[String]) =  {

        var tree_data: TreeDataLoader = null
        if (args.length != 1)
            println("Usage: scala Quantizer <filepath to tree data>")
        else {
            println("Starting service...")
            tree_data = new TreeDataLoader(args(0))
            println("Tree data loaded.")
        }
    }    
}




