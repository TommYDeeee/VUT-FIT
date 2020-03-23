package ija.ija2019.homework2.maps;

import java.util.Objects;

public class Coordinate {

    private int x,y;

    public Coordinate(int x, int y) {
        this.x=x;
        this.y=y;
    }

    public static Coordinate create(int x, int y) {
        if (x >= 0 && y >= 0) {
            return new Coordinate(x, y);

        }else{
            return null;
        }
    }

    public int getX()
    {
        return x;
    }

    public int getY()
    {
        return y;
    }

    public int diffY(Coordinate c){
        return this.y - c.y;
    }

    public int diffX(Coordinate c) {
        return this.x - c.x;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Coordinate that = (Coordinate) o;
        return x == that.x &&
                y == that.y;
    }

    @Override
    public int hashCode() {
        return Objects.hash(x, y);
    }
}
