package ija.ija2019.homework2.maps;

import java.util.Arrays;
import java.util.List;

public interface Street {

    static Street defaultStreet(String ID, Coordinate coordinate1, Coordinate coordinate2, Coordinate coordinate3) {
        MyStreet street = new MyStreet(ID, coordinate1, coordinate2, coordinate3);
        if((street.begin().getX() - coordinate2.getX())!= 0){
            return null;
        }
        if((street.end().getY() - coordinate2.getY()) != 0){
            return null;
        }
        return new MyStreet(ID,coordinate1, coordinate2, coordinate3);
    }

    static Street defaultStreet(String ID, Coordinate coordinate1, Coordinate coordinate2) {
        return new MyStreet(ID,coordinate1,coordinate2);
    }

    public String getId();

    public Coordinate begin();

    public Coordinate end();

    public List<Coordinate> getCoordinates();

    public boolean follows(Street s2);

    public boolean addStop(Stop stop);

    public List<Stop> getStops();
}
