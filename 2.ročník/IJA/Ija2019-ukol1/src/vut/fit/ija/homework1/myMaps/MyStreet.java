/*
 * Zdrojové kódy josu součástí zadání 1. úkolu pro předmětu IJA v ak. roce 2019/2020.
 * (C) Radek Kočí
 */
package vut.fit.ija.homework1.myMaps;

import java.util.ArrayList;
import java.util.List;
import vut.fit.ija.homework1.maps.Stop;
import vut.fit.ija.homework1.maps.Coordinate;
import vut.fit.ija.homework1.maps.Street;
import java.util.Arrays;

/**
 * Reprezentuje jednu ulici v mapě. Ulice má svůj identifikátor (název) a je definována souřadnicemi. Pro 1. úkol
 * předpokládejte pouze souřadnice začátku a konce ulice.
 * Na ulici se mohou nacházet zastávky.
 * @author koci
 */
public class MyStreet implements Street {
    String ID;
    Coordinate coordinate1;
    Coordinate coordinate2;
    List<Stop> list = new ArrayList<Stop>();

    public MyStreet (String ID, Coordinate coordinate1, Coordinate coordinate2)
    {
        this.ID = ID;
        this.coordinate1 = coordinate1;
        this.coordinate2 = coordinate2;
        //this.list = Arrays.asList();
    }
    /**
     * Vrátí identifikátor ulice.
     * @return Identifikátor ulice.
     */
    public String getId(){
        return ID;
    }
    
    /**
     * Vrátí seznam souřadnic definujících ulici. První v seznamu je vždy počátek a poslední v seznamu konec ulice.
     * @return Seznam souřadnic ulice.
     */
    
    public List<Coordinate> getCoordinates(){
        return Arrays.asList(coordinate1, coordinate2);
    }
    
    /**
     * Vrátí seznam zastávek na ulici.
     * @return Seznam zastávek na ulici. Pokud ulize nemá žádnou zastávku, je seznam prázdný.
     */
    public List<Stop> getStops(){
        return list;
    }
    
    /**
     * Přidá do seznamu zastávek novou zastávku.
     * @param stop Nově přidávaná zastávka.
     */
    public void addStop(Stop stop){
        list.add(stop);
        stop.setStreet(this);
    }
}
