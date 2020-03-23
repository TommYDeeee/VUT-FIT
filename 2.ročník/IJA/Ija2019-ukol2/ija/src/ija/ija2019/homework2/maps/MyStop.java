package ija.ija2019.homework2.maps;

public class MyStop implements Stop {
    private String ID;
    private  Coordinate coordinate;
    private Street street;

    public MyStop (String ID, Coordinate coordinate) {
        this.ID = ID;
        this.coordinate = coordinate;
    }
    public MyStop (String ID) {
        this.ID = ID;
    }

    @Override
    public String toString() {
        return "stop(" + this.ID +')';
    }

    public String getId() {return ID;}

    public Coordinate getCoordinate() {return  coordinate;}

    public void setStreet(Street s) {
        street = s;
    }

    public Street getStreet() {return street;}

    public boolean equals(Object object) {
        if (this == object) return true;
        if (object == null || getClass() != object.getClass()) return false;
        MyStop myStop = (MyStop) object;
        return ID.equals(myStop.ID);
    }
}

