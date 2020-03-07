/*
 * Zdrojové kódy josu součástí zadání 1. úkolu pro předmětu IJA v ak. roce 2019/2020.
 * (C) Radek Kočí
 */
package vut.fit.ija.homework1.myMaps;
import java.util.ArrayList;
import vut.fit.ija.homework1.maps.Coordinate;
import vut.fit.ija.homework1.maps.Street;
import vut.fit.ija.homework1.maps.StreetMap;
import java.util.List;
/**
 * Reprezentuje jednu mapu, která obsahuje ulice.
 * @author koci
 */
public class MyStreetMap implements StreetMap {
    ArrayList<Street> list;

    public MyStreetMap() {
        list = new ArrayList<>();
    }

    /**
     * Přidá ulici do mapy.
     * @param s Objekt reprezentující ulici.
     */
    public void addStreet(Street s) {
        this.list.add(s);
    }

    
    /**
     * Vrátí objekt reprezentující ulici se zadaným id.
     * @param id Identifikátor ulice.
     * @return Nalezenou ulici. Pokud ulice s daným identifikátorem není součástí mapy, vrací null.
     */
    public Street getStreet(String id){
        for (int i = 0; i < list.size() ; i++) {
           if (list.get(i).getId() == id)
           {
               return list.get(i);
           }
        }
        return null;
    }
}
