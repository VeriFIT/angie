
/*
objekt -hvhrana----> hodnota ->pthrana------>objekt
-size  -typ                  -offset -targetSpec

objekt -hvhrana----> hodnota
objekt -pthrana----  hodnota --------------->objekt
*/


  //! objectid je nutné, protože value je nutně vázána ke konkr. offsetu (ne nutně typu)!
  //! eg na každý možný offset v OBJ[objid] potenciálně existuje unikátní PT hrana 
  //! (a offset může být i mimo objekt, tedy záporný, nebo > sizeof)
  
  /*
17-03-02 19 UTC
target-specifier nahradit za port
 - je třeba víc promyslet
Pt součástí hodnoty typu pointer - možná dělá i predátor
Reference counter X přichozí hrany
  x pozor na cykly - > to zůstane pořád alespon 1 pointer
  x průchod z globálních prom / stacku, hledám cestu k objektu na kteýr sem ztratil jinak asi přístup
  x optimalizace, hlavní myšlenka: je na mě hrana stacku/globalu- -> nekontroluji, jinak ano
  x optimalizace: hledat zpátky cestu k stacku
probráno
COW - lze přes ID
smg - multidimenzionalni problem: čas x větvění (x uroven objektů)
PP - připomíná že by opravdu rád prošpikoval predátora analýzou a zjistil 
   - podrobnější statistiky, o prům. počtu ukaztelů na hodnotu např.
Printer - keep it simple, nemusí být efektivné, hlavne elegnantní
        - připomínám návrh Rekurzivní visitor + Printer Visitor -> rekurzivní printer visitor

*/


/*
Program 1:
x = malloc();
y = x;
free(x);
x = malloc();
if (x == y) { ERROR }


Program 2:
x = malloc();
y = x;
x = malloc();
free(y);
if (x == y) { ERROR }


první příklad, chyba může nastat - znovupřidelení
druhý příklad - nelze

Možnost optimalizovat (krom freed => možno rovnat s čímkoliv, adresu), 
přidat NEQ hrany se všemi eixstujicim objekty
a) při alokaci
b) při de-alokaci

*/


/*

Meeting log
17-02-23 17:00 UTC, Reykjavík, Brno

To discuss:
* smg - graph functions naming conventions
* design - printing library
* license - LGPLv3 (we want to be more permissive ATM)
* license - most other software is BSD-like or Appache2-like, LLVM considering Appache2/GPL2
* license - printing library is GPLv3 - what should we do if we youse it
* next week @charvin - rework the printing library predator_wrapper in a visitor like way.
-----

questions:
* Can we consider two pairs <object, target_specifier> "not pointing to the same object" if only the
  target_specifiers differs? -> question is realted to object wrappers, which could replace the role
  of target_specifier, but would introduce additional "object_id".  -> Same object accessed through
  different wrappers could not be considerd "same" based on object_id

* Is there always only on points-to edge (as per the SMG paper) pointing to object?

* How is the translation from "pointers" to "objects" done?
* How should we proceed when a pointer to an object is artihmeticaly modified in a way SMGs does not
  know the pointer (array indexing os weird sort)?  Should we search the ValueContainer for all connected
  values to the pointer and try to identify the object base pointer?
* -> OWN ANSWER: All pointer manipulation should be monitored by the SMG abstraction - so "not knowing"
  *should* not happen.  Expalanation: When creating a new pointer using computer arithmetic, a new
  PT-Edge should be created, pointing to original object with different offset.  If the arithmetic is
  done "manually" by converting the pointer to integer and back than "SMG's are blind to this" and the
  only way would be to search the ValueContainer or claiming pointer to unknown address.

*/
