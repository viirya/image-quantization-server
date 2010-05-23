
package org.viirya.quantizer

import scala.io.Source


case class MTree(id: Int, value: Array[Float], var children: List[MTree]) {
    def this(id: Int, value: Array[Float]) = this(id, value, List())
    def isLeaf(): Boolean = {
        if (children.length == 0)
            true
        else
            false
    }
    def calculateDistance(vector: Array[Float]): Float = {
        var distance: Float = 0.0f
        for (i <- 0.to(value.length - 1)) {
            distance = distance + Math.pow(value(i) - vector(i), 2).toFloat 
        }
        distance
    }                                
    def findClosetNode(vector: Array[Float]): MTree = {
        var minDistance = Math.MAX_FLOAT
        var closetNode: MTree = null
        children.foreach { child_node =>
            var distance = child_node.calculateDistance(vector)
            if (distance < minDistance) {
                minDistance = distance
                closetNode = child_node
            }
        }
        closetNode
    }
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

    var tree: List[MTree] = List()
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
                case t =>
                    var node_value: Array[Float] = split_line.map( (s) => s.toFloat ).toArray
                    var new_node: MTree = null 

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
            if (line_counter % 100 == 0)
                println("Processing line " + line_counter)
            line_counter += 1
        }
}


class Query(var values: List[Array[Float]]) {
    def this() = this(List())   
    def loadFromFile(filepath: String) = {
        var feature_dimension = 128
        var num_features = 0
        var line_counter = 0
        values = List()
        Source.fromFile(filepath)
            .getLines
            .foreach { read_line =>
                var line = read_line.stripLineEnd
                var split_line: List[String] = List.fromString(line, ' ')
                line_counter match {
                    case 0 => feature_dimension = split_line(0).toInt
                    case 1 => num_features = split_line(0).toInt
                    case _ =>
                        var feature: Array[Float] = split_line.map( (s) => s.toFloat ).toArray
                        values = feature :: values
                }
                println("line " + line_counter + " loaded.")
                line_counter = line_counter + 1
            }
    }
}

object Quantizer {

    def quantize(query: Query, tree: MTree): String = {

        var quantized_result: String = ""
        
        for (feature_point <- query.values) {
            var trace_tree = tree
            while (!trace_tree.isLeaf()) {
                trace_tree = trace_tree.findClosetNode(feature_point)
            }                
            quantized_result = quantized_result + " vec" + trace_tree.id
        }

        quantized_result 
    }

    def main(args: Array[String]) =  {

        var tree_data: TreeDataLoader = null
        if (args.length == 0)
            println("Usage: scala Quantizer <filepath to tree data> [SIFT feature file]")
        else {
            println("Starting service...")
            tree_data = new TreeDataLoader(args(0))
            println("Tree data loaded.")

            var query: Query = null
            if (args.length == 2) {
                query = new Query()
                query.loadFromFile(args(1))
            
                println(quantize(query, tree_data.root))
            }            
        }
    }    
}




