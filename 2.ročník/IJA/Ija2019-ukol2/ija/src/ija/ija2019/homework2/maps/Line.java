package ija.ija2019.homework2.maps;

import java.util.Arrays;

public interface Line {
    public static Line defaultLine(String s) {
        return new MyLine(s);
    }

    public boolean addStop(Stop stop3);

    public boolean addStreet(Street s2);

    java.util.List<java.util.AbstractMap.SimpleImmutableEntry<Street,Stop>> getRoute();
}
