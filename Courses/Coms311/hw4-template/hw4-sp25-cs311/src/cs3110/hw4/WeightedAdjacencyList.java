package cs3110.hw4;

import java.util.*;

public class WeightedAdjacencyList<T> implements WeightedGraph<T> {
    //Need to store the graph as a map, essentially a map of maps
    private final Map<T,Map<T,Integer>> adjacencyList;
    /**
     * Constructor that creates the graph given a list of vertices
     * @param initializes graph with list of vertices
     */
    public WeightedAdjacencyList(List<T> vertices) {
        // Initialize the adjacency list as a HashMap for O(1) lookups
        adjacencyList = new HashMap<>();
        
        // Add each vertex with an empty map of neighbors
        for (T vertex : vertices) {
            adjacencyList.put(vertex, new HashMap<>());
        }
    }

    /**
     * Adds the directed edge (u,v) to the graph. If the edge is already present, it should not be modified.
     * @param u The source vertex.
     * @param v The target vertex.
     * @param weight The weight of the edge (u,v).
     * @return True if the edge was added to the graph, false if 1) either u or v are not in the graph 2) the edge was already present.
     */
    @Override
    public boolean addEdge(T u, T v, int weight) {
    // Check if both vertices exist in the graph
    if (!hasVertex(u) || !hasVertex(v)) {
        return false;

    }
    // Check if the edge already exists
    if (hasEdge(u, v)) {
        return false;

    }

    // Add the edge by mapping the destination vertex to its weight
    adjacencyList.get(u).put(v, weight);
    return true;
}
        
    

    /**
     * @param vertex A vertex to add to the graph.
     * @return False vertex was already in the graph, true otherwise.
     */
    @Override
    public boolean addVertex(T vertex) {

         // Check if the vertex already exists
         if (hasVertex(vertex)) {
            return false;
        }
        
        // Add the vertex with an empty map of neighbors
        adjacencyList.put(vertex, new HashMap<>());
        return true;
    }
    

    /**
     * Returns the number of vertices in the graph.
     * @return Number of vertices.
     */
    @Override
    public int getVertexCount() {

        return adjacencyList.size();
    }


    /**
     * @param v The name of a vertex.
     * @return True if v is in the graph, false otherwise.
     */
    @Override
    public boolean hasVertex(T v) {

        return adjacencyList.containsKey(v);
    }

    /**
     * @return An Iterable of V.
     */
    @Override
    public Iterable<T> getVertices() {

        return adjacencyList.keySet();
    }

    /**
     * @return |E|
     */
    @Override
    public int getEdgeCount() {

        int count = 0;
        for (Map<T, Integer> edges : adjacencyList.values()) {
            count += edges.size();
        }

        return count;
    }

    /**
     * @param u The source of the edge.
     * @param v The target of the edge.
     * @return True if (u,v) is in the graph, false otherwise.
     */
    @Override
    public boolean hasEdge(T u, T v) {

        return hasVertex(u) && adjacencyList.get(u).containsKey(v);
    }

    /**
     * @param u A vertex.
     * @return The neighbors of u in the weighted graph.
     */
    @Override
    public Iterable<T> getNeighbors(T u) {

        if (!hasVertex(u)) {
            // Return empty list if vertex doesn't exist
            return new ArrayList<>();
        }

        // Return the keys of the inner map (neighbor vertices)
        return adjacencyList.get(u).keySet();
    }

    /**
     * @param u
     * @param v
     * @return
     */
    @Override
    public boolean areNeighbors(T u, T v) {

        return hasEdge(u,v);
    }

    /**
     * Get the weight of an edge.
     * @param u Source vertex.
     * @param v Destination vertex.
     * @return Weight of the edge (u,v).
     * @throws IllegalArgumentException if the edge doesn't exist.
     */
    public int getWeight(T u, T v) {
        if (!hasEdge(u, v)) {
            throw new IllegalArgumentException("Edge does not exist");
        }
        
        // Return the weight stored in the inner map
        return adjacencyList.get(u).get(v);
    }

    /**
     * Uses Dijkstra's algorithm to find shortest paths.
     * @param s The source vertex.
     * @return A Mapping from all reachable vertices to their distance from s.
     */
    @Override
    public Map<T, Long> getShortestPaths(T s) {
        // Check if source vertex exists
        if (!hasVertex(s)) {
            return new HashMap<>();
        }
        
        // Map to store final shortest distances
        Map<T, Long> dist = new HashMap<>();
        
        // Priority queue for Dijkstra's algorithm
        // We use a custom class to store vertex-distance pairs and sort by distance
        PriorityQueue<VertexDistancePair> pq = new PriorityQueue<>(

            Comparator.comparingLong(vd -> vd.distance)
        );
        
        // Add the source to the queue with distance 0
        pq.add(new VertexDistancePair(s, 0L));
        
        // Set to keep track of processed vertices (those with final distances)
        Set<T> processed = new HashSet<>();
        
        // Main Dijkstra loop
        while (!pq.isEmpty()) {
            // Extract the vertex with minimum distance
            VertexDistancePair pair = pq.poll();
            T u = pair.vertex;
            long distance = pair.distance;
            
            // Skip if already processed (we might have duplicate entries in the queue)
            if (processed.contains(u)) {
                continue;
            }
            
            // Mark as processed and record final distance
            processed.add(u);
            dist.put(u, distance);
            
            // Process all neighbors of the current vertex
            for (T v : adjacencyList.get(u).keySet()) {
                // Only process unprocessed neighbors
                if (!processed.contains(v)) {
                    // Calculate new potential distance through current vertex
                    long newDist = distance + getWeight(u, v);
                    
                    // If new distance is shorter than current known distance, update
                    if (!dist.containsKey(v) || newDist < dist.get(v)) {

                        // Add to queue with new distance
                        pq.add(new VertexDistancePair(v, newDist));
                    }
                }
            }
        }
        
        // Return map of shortest distances
        return dist;
    }
    
    // Helper class for Dijkstra's algorithm to store a vertex and its tentative distance
    private class VertexDistancePair {

        T vertex;
        
        long distance;
        
        VertexDistancePair(T vertex, long distance) {

            this.vertex = vertex;
            this.distance = distance;
        }
    }
}