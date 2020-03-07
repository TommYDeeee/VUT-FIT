/*
 * Zdrojové kódy josu součástí zadání 1. úkolu pro předmětu IJA v ak. roce 2019/2020.
 * (C) Radek Kočí
 */
package vut.fit.ija.homework1.myMaps;

import vut.fit.ija.homework1.maps.Coordinate;
/**
 * Reprezentuje pozici (souřadnice) v mapě. Souřadnice je dvojice (x,y), počátek mapy je vždy na pozici (0,0). 
 * Nelze mít pozici se zápornou souřadnicí.
 * @author koci
 */
public class MyCoordinate implements Coordinate {
    private int x,y;

    public MyCoordinate(int x, int y) {
        this.x=x;
        this.y=y;
    }


    public static MyCoordinate create(int x, int y){
        if (x >= 0 && y >= 0) {
            return new MyCoordinate(x, y);

        }else{
            return null;
        }

    }
    /**
     * Vrací hodnotu souřadnice x.
     * @return Souřadnice x.
     */
    public int getX()
    {
        return x;
    }

    /**
     * Vrací hodnotu souřadnice y.
     * @return Souřadnice y.
     */
    public int getY()
    {
        return y;
    }


    public boolean equals(Object object) {
        if (this == object) return true;
        if (object == null || getClass() != object.getClass()) return false;
        MyCoordinate that = (MyCoordinate) object;
        return x == that.x &&
                y == that.y;
    }
}
