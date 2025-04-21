package cs3110.hw4;

import java.util.Objects;

/**
 * A simple class representing a 2-tuple.
 * @param <S>
 * @param <T>
 */
public class Pair<S, T> {
    private final S first;
    private final T second;

    public Pair(S first, T second) {
        this.first = first;
        this.second = second;
    }

    public S getFirst() {
        return first;
    }

    public T getSecond() {
        return second;
    }

    @Override
    public int hashCode() {
        int a = first != null ? first.hashCode() : 0;
        int b = second != null ? second.hashCode() : 0;
        return a + 31 * b;
    }

    @SuppressWarnings("rawtypes")
    @Override
    public boolean equals(Object obj) {
        if (obj == null || this.getClass() != obj.getClass()) return false;

        Pair other = (Pair) obj;
        return (Objects.equals(this.first, other.first)) &&
                (Objects.equals(this.second, other.second))
                ;
    }

    public static <S, T> Pair<S, T> create(S first, T second) {
        return new Pair<>(first, second);
    }
}
