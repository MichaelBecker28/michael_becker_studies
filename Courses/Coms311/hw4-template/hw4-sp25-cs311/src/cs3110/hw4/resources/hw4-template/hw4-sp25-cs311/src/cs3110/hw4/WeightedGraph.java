package cs3110.hw4;

public interface WeightedGraph<T> extends Graph<T> {
    public boolean addEdge(T u, T v, int weight);

    public default boolean addEdge(T u, T v) {
        return addEdge(u, v, 1);
    }
}
