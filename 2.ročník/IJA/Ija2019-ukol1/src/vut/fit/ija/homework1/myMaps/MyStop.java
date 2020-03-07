/*
 * Zdrojové kódy josu součástí zadání 1. úkolu pro předmětu IJA v ak. roce 2019/2020.
 * (C) Radek Kočí
 */
package vut.fit.ija.homework1.myMaps;
import vut.fit.ija.homework1.maps.Stop;
import vut.fit.ija.homework1.maps.Coordinate;
import vut.fit.ija.homework1.maps.Street;
/**
 * Reprezentuje zastávku. Zastávka má svůj unikátní identifikátor a dále souřadnice umístění a zná ulici, na které je umístěna.
 * Zastávka je jedinečná svým identifikátorem. Reprezentace zastávky může existovat, ale nemusí mít
 * přiřazeno umístění (tj. je bez souřadnic a bez znalosti ulice). Pro shodu objektů platí, že dvě zastávky jsou shodné, pokud
 * mají stejný identifikátor.
 * @author koci
 */
public class MyStop implements Stop{
    private String ID;
    private Coordinate coordinate;
    private Street street;

    public MyStop (String ID, Coordinate coordinate)
    {
        this.ID = ID;
        this.coordinate = coordinate;
    }

    public MyStop (String ID)
    {
        this.ID = ID;
    }

    /**
     * Vrátí identifikátor zastávky.
     * @return Identifikátor zastávky.
     */
    public String getId() {return ID;}
    
    /**
     * Vrátí pozici zastávky.
     * @return Pozice zastávky. Pokud zastávka existuje, ale dosud nemá umístění, vrací null.
     */
    public Coordinate getCoordinate() {return  coordinate;}

    /**
     * Nastaví ulici, na které je zastávka umístěna.
     * @param s Ulice, na které je zastávka umístěna.
     */
    public void setStreet(Street s)
    {
        street = s;
    }

    /**
     * Vrátí ulici, na které je zastávka umístěna.
     * @return Ulice, na které je zastávka umístěna. Pokud zastávka existuje, ale dosud nemá umístění, vrací null.
     */
    public Street getStreet() {return street;}

    public boolean equals(Object o) {
        if(o instanceof Stop) {
            return this.ID == ((Stop)o).getId();
        } else {
            return false;
        }
    }
}
