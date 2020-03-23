package ija.ija2019.homework2.maps;

import java.util.*;

public class MyLine implements Line{
    private String ID;
    ArrayList<Stop> list;
    ArrayList<Street> list_s;
    List<AbstractMap.SimpleImmutableEntry<Street, Stop>> Line;


    public MyLine(String ID) {
        this.ID = ID;
        list = new ArrayList<Stop>();
        list_s = new ArrayList<Street>();
        Line = new ArrayList<>();
    }

    @Override
    public boolean addStop(Stop stop3) {
        if(this.list.size() == 0){
            list.add(stop3);
            list_s.add(stop3.getStreet());
            Line.add(new AbstractMap.SimpleImmutableEntry<>(stop3.getStreet(), stop3));
            return true;
        }

        if(stop3.getStreet().follows(this.list_s.remove(0))){
            list.add(stop3);
            list_s.add(stop3.getStreet());
            Line.add(new AbstractMap.SimpleImmutableEntry<>(stop3.getStreet(), stop3));
            return true;
        }
        else {
            return false;
        }
    }

    @Override
    public boolean addStreet(Street s2) {
        list_s.add(s2);
        if(s2.getStops().isEmpty()){
            Line.add(new AbstractMap.SimpleImmutableEntry<>(s2, null));
        }
        return true;
    }

    @Override
    public List<AbstractMap.SimpleImmutableEntry<Street, Stop>> getRoute() {
        return Collections.unmodifiableList(this.Line);
    }
}
