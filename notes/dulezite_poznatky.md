Konstanty jsou v reprezentaci IR unikatni:
------------------------------------------
priklad:  
> intova_funkce(10,20);  
> func_ptr(20,60);

vygeneruje in-memory llvm IR se stejnou value
pro konstantu 20 (stejny ukazatel)

Systém reprezentace hodnot v frameworku
----------------------------------
* Jedna reprezentace hodnoty = popis číslené hodnoty pomocí různých prostředů eg.:
  * přesné přiřazení konstanty
  * intervalů
  * binárních relací mezi reprezentacemi hodnot
  * nastavených bitů
  * ...
* Všechny reprezentované hodnoty budou v prvním návrhu ve sdíleném prostoru
* Mělo by se zabránit vytváření duplicitní reprezentací (popisujících tutéž množinu čísel)
* Operace rovnosti a nerovnosti přímo nad reprezentacemi hodnot by měli pracovat ve smyslu "platí pro všechny možnost". Příklad:
  > VR_X = {10,11,12,13,14}  
  > VR_Y = {13,14,15}  
  > VR_Z = {15,16}  
  > VR_X == VR_Y -> unknown  
  > VR_X == VR_Z -> false  
  > VR_X != VR_Z -> true  
  > VR_Y == VR_Z -> unknown  
* V případě binárních relací ->> ERRR sakra!!!
  > 
