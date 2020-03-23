package ija.ija2019.homework2.maps;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MyStreet implements Street {
    String ID;
    Coordinate coordinate1;
    Coordinate coordinate2;
    Coordinate coordinate3;
    ArrayList<Stop> list;

    public MyStreet (String ID, Coordinate coordinate1, Coordinate coordinate2, Coordinate coordinate3)
    {
        this.ID = ID;
        this.coordinate1 = coordinate1;
        this.coordinate2 = coordinate2;
        this.coordinate3 = coordinate3;
        list = new ArrayList<Stop>();
    }

    public MyStreet(String ID, Coordinate coordinate1, Coordinate coordinate2) {
        this.ID = ID;
        this.coordinate1 = coordinate1;
        this.coordinate2 = coordinate2;
        list = new ArrayList<Stop>();
    }

    public Coordinate begin() {
        return getCoordinates().get(0);
    }

    public Coordinate end() {
        if(getCoordinates().get(2) == null){
            return getCoordinates().get(1);
        }
        return getCoordinates().get(2);
    }

    @Override
    public String toString() {
        return ID;
    }

    public boolean follows(Street street) {
        if(this.begin().diffX(street.begin()) == 0 && this.begin().diffY(street.begin()) == 0){
            return  true;
        }
        else if(this.begin().diffX(street.end()) == 0 && this.begin().diffY(street.end()) == 0){
            return true;
        }
        else if(this.end().diffX(street.begin()) == 0 && this.end().diffY(street.begin()) == 0){
            return true;
        }
        else if(this.end().diffX(street.end()) == 0 && this.end().diffY(street.begin()) == 0){
            return true;
        }
        else {
            return false;
        }

    }
    public String getId(){
        return ID;
    }

    public List<Coordinate> getCoordinates(){
        return Arrays.asList(coordinate1, coordinate2, coordinate3);
    }


    public List<Stop> getStops(){
        return list;
    }

    public boolean addStop(Stop stop){
        int a = (int) Math.sqrt(Math.pow((this.begin().getX() - stop.getCoordinate().getX()), 2) + Math.pow((this.begin().getY() - stop.getCoordinate().getY()), 2));
        int b = (int) Math.sqrt(Math.pow((stop.getCoordinate().getX() - this.coordinate2.getX()), 2) + Math.pow((stop.getCoordinate().getY() - this.coordinate2.getY()), 2));
        int c = (int) Math.sqrt(Math.pow((this.begin().getX() - this.coordinate2.getX()), 2) + Math.pow((this.begin().getY() - this.coordinate2.getY()), 2));
        if( a + b == c){
            list.add(stop);
            stop.setStreet(this);
            return true;
        }
        if(coordinate3 != null){
            a = (int) Math.sqrt(Math.pow((this.coordinate2.getX() - stop.getCoordinate().getX()), 2) + Math.pow((this.end().getY() - stop.getCoordinate().getY()), 2));
            b = (int) Math.sqrt(Math.pow((stop.getCoordinate().getX() - this.end().getX()), 2) + Math.pow((stop.getCoordinate().getY() - this.end().getY()), 2));
            c = (int) Math.sqrt(Math.pow((this.coordinate2.getX() - this.end().getX()), 2) + Math.pow((this.coordinate2.getY() - this.end().getY()), 2));
            if( a + b == c){
                list.add(stop);
                stop.setStreet(this);
                return true;
            }
        }
        return false;
    }
}
