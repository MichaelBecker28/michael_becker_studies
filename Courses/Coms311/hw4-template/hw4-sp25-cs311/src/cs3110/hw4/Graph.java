package cs3110.hw4;

import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * An interface for implementing various operations on connected weighted graphs.
 * Vertices are identified by Strings. Equal strings <=> same vertex.
 * Weights are non-negative integers.
 * Note that some documentation may use an overloaded definition for [i], such as [i] for List.get(i).
 */
public interface Graph<T> {

    public boolean addVertex(T vertex);

    public boolean addEdge(T u, T v);

    /**
     *
     * @return |V|
     */
    public int getVertexCount();

    /**
     *
     * @param v The name of a vertex.
     * @return True if vertex v is present in the graph, false otherwise.
     */
    public boolean hasVertex(T v);

    /**
     * Get every vertex in the graph.
     * @return An object that is iterable over every vertex in the graph.
     */
    public Iterable<T> getVertices();

    /**
     *
     * @return |E|
     */
    public int getEdgeCount();

    /**
     *
     * @param u One endpoint of the edge.
     * @param v The other endpoint of the edge.
     * @return True if edge (u,v) is present in the graph, false otherwise.
     */
    public boolean hasEdge(T u, T v);

    /**
     * Returns all neighbors of vertex u.
     * @param u A vertex.
     * @return The neighbors of u.
     */
    public Iterable<T> getNeighbors(T u);

    public boolean areNeighbors(T u, T v);

    /**
     * Finds the length of the shortest path from vertex s to all other vertices in the graph.
     * @param s The source vertex.
     * @return A map of shortest path distances. For key value pair (k,v), v is the length of the shortest s->k path or -1 if no such path exists.
     */
    public Map<T, Long> getShortestPaths(T s);
}
