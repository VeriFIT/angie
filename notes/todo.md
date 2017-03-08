* Hodnotový systém !
* Úložiště ladících informací / informacích o kódu obecně
* Adresace položek v aggregate typech, zejména v případě abstraktního indexu
  > Je potřeba nakreslit si strukturu a ukazatel na ní v SMG  
  > To samé pak i se zanořenými stukturami (myšleno přímo)  
  > A také se uniemi  
  > Poté vyjádřit spojení jednotlivých indexů funkce getelementptr případně funkce --to-samé-pro-hodnoty-v-registru--
* Visitor-like-model pro ukládání zpráv do fronty
* Dummy abstrakční doména:
  > Výpis vzniku hodnoty  
  > Výpis vzniku aliasu k hodnotě (bitcast)  
  > Výpis přístupu k prvku aggregate  
  > Výpis volání funkcí
  > Výpis volání speciálních funkcí (alokace, ...)
  > 
* Vybrat vhodou C++ knihovnu pro strem-api, lazy evaluation a ranges
* Otestovat binding C# na C++ (STL i boost)

* Vyrobit kontejner pro mapování FrontedValueId na ValueId
* Projít materialy k statické analýze
* vyhodnotit vývoj FNA analýzy
* přejmenovat některá místa z verifikace na anlýzu
* je tento framework určen (ze všech statických analýz) konkrétně na abstraktní interpretaci?
* probrat některý tyto věci @TomášVojnar

* umoznit null-assigned hook v ValueContainer
* implementovat něco jeko počítadlo generací ve stavu, aby bylo možné organizovat BFS/DFS atp
*
