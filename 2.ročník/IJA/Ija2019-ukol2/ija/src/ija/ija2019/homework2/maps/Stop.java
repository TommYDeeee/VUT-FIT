package ija.ija2019.homework2.maps;

public interface Stop {

    public static Stop defaultStop(String ID, Coordinate c) {
        return new MyStop(ID, c);
    }

    public String getId();
    public Coordinate getCoordinate();

    public void setStreet(Street s);

    public Street getStreet();
}
