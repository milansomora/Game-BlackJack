// BlackJack Kartenspiel in C++
// Konzolová karetní hra BlackJack v C++
// Milan Somora

#include "stdafx.h"  // knihovna pro Visual Studio
#include <algorithm> // random_shuffle
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <iostream>  // cout, cin
#include <sstream>   // paměťové proudy
#include <string>    // řetězce
#include <vector>    // dynamické pole
#include <windows.h> // SetConsoleTextAttribute

// Barvy
#define black       0
#define dark_red    4
#define grey        7
#define yellow      14

using namespace std;
typedef unsigned short USHORT;

// Prototypy funkcí
void BarevnyVystup(int, int, int, const string);
void PrevedNaKartu(USHORT&, stringstream&);
void SkoreSoucet(USHORT&, USHORT&);

class GeneratorNahodnychCisel {
public:
	static USHORT Cislo(USHORT velikostVektoru) {
		srand(unsigned(time(0)));
		return rand() % velikostVektoru;
	}
};

class BalicekKaret {
public:
	BalicekKaret();
	~BalicekKaret() {}

	void   ZamichejKarty();
	USHORT OdeberKartu();

private:
	vector<USHORT> karty;
};

// Vytvoří balíček
BalicekKaret::BalicekKaret() {
	this->karty.clear();

	// Naplní kontejner kartami 2 - 10, 11 = A
	for (USHORT i = 2; i < 12; i++) {
		this->karty.push_back(i);
	}

	// Naplní kontejner kartami J, Q, K, A - znak bude převeden na kód ASCII
	this->karty.push_back('J'); // J = 74
	this->karty.push_back('Q'); // Q = 81
	this->karty.push_back('K'); // K = 75
}

// Odebere kartu
USHORT BalicekKaret::OdeberKartu() {
	USHORT posledniKarta = this->karty[this->karty.size() - 1];
	this->karty.pop_back();
	return posledniKarta;
}

// Zamíchá karty v balíčku
void BalicekKaret::ZamichejKarty() {
	random_shuffle(this->karty.begin(), this->karty.end(), GeneratorNahodnychCisel::Cislo);
}

// Abstraktkní třída
class Hraci {
public:
	Hraci() {}
	virtual ~Hraci() {}

	virtual void   VemKartu(USHORT&) = 0;
	virtual void   UlozSkore(USHORT&) = 0;
	virtual USHORT ZiskejSkore() const = 0;
	virtual void   UkazKarty() const = 0;

protected:
	USHORT skore;
	vector<USHORT> karty;
};

// Abstraktkní třída Počtář
class Poctar {
public:
	virtual void SpocitejSkore() = 0;
};

class Krupier : public Hraci, public Poctar {
public:
	Krupier();
	~Krupier() {}

	virtual USHORT  ZiskejSkore() const { return this->skore; }
	virtual void    SpocitejSkore();
	virtual void    UlozSkore(USHORT& sk) { this->skore = sk; }
	virtual void    VemKartu(USHORT&);
	virtual void    UkazKarty() const;
	BalicekKaret balicekKaret; // Třída jako atribut
};

Krupier::Krupier() {
	this->skore = 0;
	this->karty.clear();
}

void Krupier::VemKartu(USHORT& k) {
	this->karty.push_back(k);
}

void Krupier::UkazKarty() const {
	stringstream proud; // Paměťový proud
	string retezecKarty;
	USHORT pom;

	for (vector<USHORT>::const_iterator i = this->karty.begin();
		i != this->karty.end(); i++) {
		pom = *i;
		PrevedNaKartu(pom, proud);
	}
	retezecKarty = proud.str();                 // Převede paměťový proud na řetězec typu string
	retezecKarty.erase(retezecKarty.end() - 2); // Odstraní poslední znak z řetězce
	BarevnyVystup(0, 2, 7, retezecKarty);       // Vypíše řetězec na stdout v zelené barvě
}

void Krupier::SpocitejSkore() {
	USHORT soucet = 0;
	USHORT pom;

	for (vector<USHORT>::const_iterator i = this->karty.begin();
		i != this->karty.end(); i++) {
		pom = *i;
		SkoreSoucet(soucet, pom);
	}
	this->skore = soucet;
}

class Hrac : public Hraci, public Poctar {
public:
	Hrac();
	~Hrac() {}

	virtual USHORT ZiskejSkore() const { return this->skore; }
	virtual void   SpocitejSkore();
	virtual void   UlozSkore(USHORT& sk) { this->skore = sk; }
	virtual void   VemKartu(USHORT&);
	virtual void   UkazKarty() const;
	void           vlozDoBanku(int&);
	void           OdeberZBanku(int&);
	int            SpocitejBank();

	int sazka;
	bool hracBereKartu;
	bool hracZvolilDouble;
	string jmenoHrace;

private:
	vector<int> bank;
};

Hrac::Hrac() {
	this->karty.clear();
	this->bank.clear();
	this->sazka = 0;
	this->skore = 0;
	this->jmenoHrace = "Hrac";
	hracBereKartu = false;
	hracZvolilDouble = false;
}

void Hrac::VemKartu(USHORT& k) {
	this->karty.push_back(k);
}

void Hrac::UkazKarty() const {
	stringstream proud;
	string retezecKarty;
	USHORT pom;

	for (vector<USHORT>::const_iterator i = this->karty.begin(); i != this->karty.end(); i++) {
		pom = *i;
		PrevedNaKartu(pom, proud);
	}
	retezecKarty = proud.str();
	retezecKarty.erase(retezecKarty.end() - 2);
	BarevnyVystup(0, 6, 7, retezecKarty);
}

void Hrac::SpocitejSkore() {
	USHORT soucet = 0;
	USHORT pom;

	for (vector<USHORT>::const_iterator i = this->karty.begin(); i != this->karty.end(); i++) {
		pom = *i;
		SkoreSoucet(soucet, pom);
	}
	this->skore = soucet;
}

void Hrac::vlozDoBanku(int& saz) {
	this->bank.push_back(saz);
}

void Hrac::OdeberZBanku(int& saz) {
	this->bank.push_back(saz * (-1)); // Přidá do banku zaporné čislo
}

int Hrac::SpocitejBank() {
	int soucet = 0;

	for (vector<int>::const_iterator i
		= this->bank.begin(); i != this->bank.end();
		i++) {
		soucet += *i;
	}
	return soucet;
}

// Prototypy funkcí
bool KontrolaPrazdnehoBanku(Krupier *, Hrac *);
void VyhodnotHru(Krupier *ukKrupier, Hrac *ukHrac);

class Manazer {
public:
	Manazer() {}
	~Manazer() {}

	static void ZobrazHlavniMenu();
	static void ZobrazPravidlaHry();
	static void DotazNaHraceHra(Krupier *ukKrupier, Hrac *ukHrac);
	static void DotazNaHraceKonecHry(Krupier *ukKrupier, Hrac *ukHrac);
	static void SpustHru(Krupier *ukKrupier, Hrac *ukHrac, bool);
};

void Manazer::SpustHru(Krupier *ukKrupier, Hrac *ukHrac, bool novaHra = false) {
	system("CLS");

	// Počáteční vklad do banku a jmeno hrace
	if (!novaHra) {
		int castka = 5000;
		ukHrac->vlozDoBanku(castka);

		// Jmeno hráče
		cout << "\Zadej svoje jmeno nebo prezdivku: ";
		cin >> ukHrac->jmenoHrace;
	}

	system("CLS");

	// Zadani sazky
	cout << "Mas v banku: " << ukHrac->SpocitejBank();
	cout << "\nZadej sazku: ";
	cin >> ukHrac->sazka; // Na začátku hry byla sázka hráče 0 - viz implicitní konstruktor

	while (ukHrac->sazka<100 || ukHrac->sazka> ukHrac->SpocitejBank()) {
		cout << "\nZadej sazku v rozmezi 100 - " << ukHrac->SpocitejBank() << "): ";
		cin >> ukHrac->sazka;
	}

	ukHrac->OdeberZBanku(ukHrac->sazka);

	// Vypíše stav banku
	cout << "\nMas v banku: " << ukHrac->SpocitejBank();

	// Zamíchá karty
	ukKrupier->balicekKaret.ZamichejKarty();

	// Rozdá karety na začátku hry
	cout << "\n\n-Rozdavam karty-";
	USHORT karta; // Pomocná proměnná

				  // Rozdává 2x po sobě
	karta = ukKrupier->balicekKaret.OdeberKartu();
	ukHrac->VemKartu(karta);
	karta = ukKrupier->balicekKaret.OdeberKartu();
	ukKrupier->VemKartu(karta);
	karta = ukKrupier->balicekKaret.OdeberKartu();
	ukHrac->VemKartu(karta);
	karta = ukKrupier->balicekKaret.OdeberKartu();
	ukKrupier->VemKartu(karta);

	// Vypíše karty
	cout << "\nKrupier: ";
	ukKrupier->UkazKarty();
	cout << endl;
	cout << ukHrac->jmenoHrace << ": ";
	ukHrac->UkazKarty();
	cout << endl;

	// Spočítá skóre
	ukKrupier->SpocitejSkore();
	ukHrac->SpocitejSkore();

	// Vypíše skore
	cout << "\n-SKORE-";
	cout << "\nKrupier: ";
	cout << ukKrupier->ZiskejSkore();
	cout << endl;
	cout << ukHrac->jmenoHrace << ": ";
	cout << ukHrac->ZiskejSkore();

	// Vyhodnotí hru podle pravidel
	cout << endl;
	VyhodnotHru(ukKrupier, ukHrac);

	cout << "\nMas v banku: " << ukHrac->SpocitejBank();
	Manazer::DotazNaHraceKonecHry(ukKrupier, ukHrac);
	cout << endl;
}

void Manazer::ZobrazPravidlaHry() {
	system("CLS");
	cout << endl;
	cout << "*** PRAVIDLA HRY BLACKJACK ***" << endl;
	cout << endl;
	cout << "1) Minimalni sazka je 100." << endl;
	cout << "2) Remiza\t- Kdyz skore krupiera = skore hrace." << endl;
	cout << "3) BlackJack\t- Ten, jehoz soucet karet je presne 21, vyhrava v pomeru 1,5:1." << endl;
	cout << "4) Double\t- Hrac zdvojnasobi sazku, vezme si poslední kartu a ukonci hru." << endl;
	cout << "\t\t  Na radu prijde krupier a rozhodne se. " << endl;
	cout << "5) Vzdat se\t- Kdyz hrac neveri svym kartam, muze se vzdat," << endl;
	cout << "\t\t  ale dostane zpet jen 1/2 sazky (druha propadne kasinu)." << endl;
	cout << "6) Pretahnuti\t- Ten, jehoz soucet presahne hodnotu 21," << endl;
	cout << "\t\t  prohrava svou sazku." << endl;
	cout << endl;
	cout << endl;
	cout << "Pravidla pro krupiera:" << endl;
	cout << endl;
	cout << "1) Pokud ma skore <16, musi si vzit dalsi kartu." << endl;
	cout << "2) Pokud ma skore >17, musi stat. " << endl;
	cout << "3) Pokud ma skore =16 nebo =17, vyhrava ten," << endl;
	cout << "   kdo ma nejvyssi skore." << endl;

	cout << "\nPro navrat stiskni klavesu 1: ";
	char volba;
	cin >> volba;

	while (volba != '1') {
		cout << "\nZadal jsi spatne cislo, opakuj: ";
		cin >> volba;
	}
	Manazer::ZobrazHlavniMenu();
}

void Manazer::ZobrazHlavniMenu() {
	system("CLS");
	cout << "Black Jack" << endl;
	cout << endl;
	cout << "(1) Nova hra" << endl;
	cout << "(2) Pravidla hry" << endl;
	cout << "(3) Ukoncit BlackJack" << endl;
	cout << endl;

	cout << "Volba: ";
	char volba;
	cin >> volba;

	switch (volba) {
	default:
		while (volba != '1' && volba != '2' && volba != '3') {
			cout << "\nZadal jsi spatne cislo, opakuj: ";
			cin >> volba;
		}
	case '1':
	{
		// Vytvoří objekty na hromadě
		Krupier *krupier = new Krupier();
		Hrac    *hrac = new Hrac();

		Manazer::SpustHru(krupier, hrac, false);
	}
	case '2':
		ZobrazPravidlaHry();
		break;
	case '3':
		exit(0);
		break; // Skočí do mainu a ukončí program
	}
}

// Dotaz na hráče v průbehu hry
void Manazer::DotazNaHraceHra(Krupier *ukKrupier, Hrac *ukHrac) {
	cout << "\n\n(1) Beru kartu, (2) Stojim, (3) Double, (4) Vzdavam se: ";
	char volba;
	cin >> volba;

	switch (volba) {
	default:

		while (volba != '1' && volba != '2' && volba != '3'
			&& volba != '4') {
			cout << "\nZadal jsi spatne cislo, opakuj: ";
			cin >> volba;
		}
	case '1':
	{
		ukHrac->hracBereKartu = true;

		// Krupiér dává kartu hráčovi
		USHORT karta = ukKrupier->balicekKaret.OdeberKartu();
		ukHrac->VemKartu(karta);

		cout << "\n-KARTY-";
		cout << "\nKrupier: ";
		ukKrupier->UkazKarty();
		cout << endl;
		cout << ukHrac->jmenoHrace << ": ";
		ukHrac->UkazKarty();

		cout << "\n\n-SKORE-";
		ukHrac->SpocitejSkore();
		cout << "\nKrupier: ";
		cout << ukKrupier->ZiskejSkore();
		cout << endl;
		cout << ukHrac->jmenoHrace << ": ";
		cout << ukHrac->ZiskejSkore();
		cout << endl;

		VyhodnotHru(ukKrupier, ukHrac);
		ukHrac->hracBereKartu = false;

		cout << "\nMas v banku: " << ukHrac->SpocitejBank();
		Manazer::DotazNaHraceKonecHry(ukKrupier, ukHrac);
	}
	case '2':
	{
		if (ukKrupier->ZiskejSkore() > 17) {
			cout << "\nZvol jiny krok, protoze krupier musi podle pravidel take stat (jeho skore <17)...";
			Manazer::DotazNaHraceHra(ukKrupier, ukHrac);
		}
		// Řada na krupiérovi
		else if (ukKrupier->ZiskejSkore() < 16) {
			cout << "\nKrupier bere dalsi kartu...";
			USHORT karta = ukKrupier->balicekKaret.OdeberKartu();
			ukKrupier->VemKartu(karta);
			ukKrupier->SpocitejSkore();
		}

		cout << "\n\n-KARTY-";
		cout << "\nKrupier: ";
		ukKrupier->UkazKarty();
		cout << endl;
		cout << ukHrac->jmenoHrace << ": ";
		ukHrac->UkazKarty();

		cout << "\n\n-SKORE-";
		ukHrac->SpocitejSkore();
		cout << "\nKrupier: ";
		cout << ukKrupier->ZiskejSkore();
		cout << endl;
		cout << ukHrac->jmenoHrace << ": ";
		cout << ukHrac->ZiskejSkore();
		cout << endl;

		VyhodnotHru(ukKrupier, ukHrac);
		cout << "\nMas v banku: " << ukHrac->SpocitejBank();
		Manazer::DotazNaHraceKonecHry(ukKrupier, ukHrac);
	}
	case '3':
	{
		ukHrac->hracZvolilDouble = true;
		ukHrac->OdeberZBanku(ukHrac->sazka);
		ukHrac->sazka *= 2;

		// Krupiér dává kartu hráčovi
		USHORT karta = ukKrupier->balicekKaret.OdeberKartu();
		ukHrac->VemKartu(karta);

		// Řada na krupiérovi
		if (ukKrupier->ZiskejSkore() < 16) {
			cout << "\nKrupier bere dalsi kartu...";
			USHORT karta = ukKrupier->balicekKaret.OdeberKartu();
			ukKrupier->VemKartu(karta);
			ukKrupier->SpocitejSkore();
		}

		cout << "\n\n-KARTY-";
		cout << "\nKrupier: ";
		ukKrupier->UkazKarty();
		cout << endl;
		cout << ukHrac->jmenoHrace << ": ";
		ukHrac->UkazKarty();

		cout << "\n\n-SKORE-";
		ukHrac->SpocitejSkore();
		cout << "\nKrupier: ";
		cout << ukKrupier->ZiskejSkore();
		cout << endl;
		cout << ukHrac->jmenoHrace << ": ";
		cout << ukHrac->ZiskejSkore();
		cout << endl;

		VyhodnotHru(ukKrupier, ukHrac);
		ukHrac->hracZvolilDouble = false;
		cout << "\nMas v banku: " << ukHrac->SpocitejBank();
		Manazer::DotazNaHraceKonecHry(ukKrupier, ukHrac);
	}
	case '4':
	{
		char volba;
		cout
			<< "\nOpravdu se chces vzdat? Prijdes o polovinu sazky! (a/n) ";
		cin >> volba;

		while (volba != 'A' && volba != 'a' && volba != 'N' && volba != 'n') {
			cout << "\nZadal jsi spatne pismeno, opakuj: ";
			cin >> volba;
		}

		if ((volba == 'A') || 'a') {
			int pomSazka = ukHrac->sazka / 2;
			ukHrac->vlozDoBanku(pomSazka);
			cout << "\nMas v banku: " << ukHrac->SpocitejBank();
			Manazer::DotazNaHraceKonecHry(ukKrupier, ukHrac);
		}
		else {
			Manazer::DotazNaHraceHra(ukKrupier, ukHrac);
		}
	}
	}
}

// Dotaz na hráče na konci hry
void Manazer::DotazNaHraceKonecHry(Krupier *ukKrupier, Hrac *ukHrac) {
	cout << "\n\n(1) Dalsi hra, (2) Ukoncit hru: ";
	char volba;
	cin >> volba;

	switch (volba) {
	default:
		while (volba != '1' && volba != '2' && volba != '3') {
			cout << "\nZadal jsi spatne cislo, opakuj: ";
			cin >> volba;
		}

	case '1':
	{
		// Kontrola dostupných prostředků v banku
		bool prazdnyBank = KontrolaPrazdnehoBanku(ukKrupier, ukHrac);

		if (prazdnyBank) {
			Manazer::ZobrazHlavniMenu;
		}
		else {
			// Zálohování hráčovo banku před další hrou
			int *minulyBank = new int;
			*minulyBank = ukHrac->SpocitejBank();

			// Vymazání objektů
			delete ukKrupier;
			delete ukHrac;

			// Opětovné vytvoreni objektů (atributy se znovu inicializují)
			Krupier *krupier = new Krupier();
			Hrac    *hrac = new Hrac();
			hrac->vlozDoBanku(*minulyBank); // Uložení hodnoty z minulé hry do banku
			delete minulyBank;
			minulyBank = nullptr;

			Manazer::SpustHru(krupier, hrac, true);
		}
	}
	case '2':
		delete ukKrupier;
		delete ukHrac;
		Manazer::ZobrazHlavniMenu();
	}
}

int main() {
	Manazer::ZobrazHlavniMenu();
	return 0;
}

void VyhodnotHru(Krupier *ukKrupier, Hrac *ukHrac) {
	// Krupiér stojí...
	if (!ukHrac->hracBereKartu && (ukKrupier->ZiskejSkore() > 17)) {
		cout << "\nKrupier stoji...";
		Manazer::DotazNaHraceHra(ukKrupier, ukHrac);
	}

	// Krupiér bere další kartu
	else if (!ukHrac->hracZvolilDouble && (ukKrupier->ZiskejSkore() < 16)) {
		cout << "\n\nKrupier bere dalsi kartu...";
		USHORT karta = ukKrupier->balicekKaret.OdeberKartu();
		ukKrupier->VemKartu(karta);

		cout << "\n\n-KARTY-";
		cout << "\nKrupier: ";
		ukKrupier->UkazKarty();
		cout << endl;
		cout << ukHrac->jmenoHrace << ": ";
		ukHrac->UkazKarty();

		cout << "\n\n-SKORE-";
		ukHrac->SpocitejSkore();
		ukKrupier->SpocitejSkore();
		cout << "\nKrupier: ";
		cout << ukKrupier->ZiskejSkore();
		cout << endl;
		cout << ukHrac->jmenoHrace << ": ";
		cout << ukHrac->ZiskejSkore();
		cout << endl;

		VyhodnotHru(ukKrupier, ukHrac);
	}

	// Remíza
	else if (ukKrupier->ZiskejSkore() == ukHrac->ZiskejSkore()) {
		BarevnyVystup(0, 4, 7, "\nREMIZA! Vracim sazku do banku..."); // vypsání řetězce na stdout v červené barvě
		ukHrac->vlozDoBanku(ukHrac->sazka);
		ukHrac->sazka = 0;
	}

	// Krupiér přetáhl
	else if (ukKrupier->ZiskejSkore() > 21) {
		BarevnyVystup(0, 4, 7, "\nVYHRAVAS! Krupier pretahl!");
		ukHrac->sazka *= 2;
		ukHrac->vlozDoBanku(ukHrac->sazka);
		ukHrac->sazka = 0;
	}

	// Hráč přetáhl
	else if (ukHrac->ZiskejSkore() > 21) {
		BarevnyVystup(0, 4, 7, "\nPROHRAVAS! Pretahl jsi!");
		ukHrac->sazka = 0;
	}

	// Krupiér má BlackJack
	else if (ukKrupier->ZiskejSkore() == 21) {
		BarevnyVystup(0, 4, 7, "\nPROHRAVAS! Krupier ma BlacjKack!");
		ukHrac->sazka = 0;
	}

	// Hráč má BlackJack
	else if (ukHrac->ZiskejSkore() == 21) {
		BarevnyVystup(0, 4, 7, "\nVYHRAVAS! Mas BlacjKack!");
		ukHrac->sazka = ukHrac->sazka + int(ukHrac->sazka * 1.5);
		ukHrac->vlozDoBanku(ukHrac->sazka);
		ukHrac->sazka = 0;
	}

	else if ((ukKrupier->ZiskejSkore() == 16) || (ukKrupier->ZiskejSkore() == 17)) {

		// Hráč má vyšší skóre než krupiér
		if (ukHrac->ZiskejSkore() > ukKrupier->ZiskejSkore()) {
			BarevnyVystup(0, 4, 7, "\nVYHRAVAS! Mas vyssi skore!");
			ukHrac->sazka *= 2;
			ukHrac->vlozDoBanku(ukHrac->sazka);
		}

		// Krupiér má vyšší skóre než hráč
		else if (ukKrupier->ZiskejSkore() > ukHrac->ZiskejSkore()) {
			BarevnyVystup(0, 4, 7, "\nPROHRAVAS! Krupier ma vyssi skore!");
			ukHrac->sazka = 0;
		}
	}

	// Krupiér má vyšší skóre než hráč
	else if (ukHrac->hracBereKartu
		&& (ukKrupier->ZiskejSkore() > ukHrac->ZiskejSkore())) {
		BarevnyVystup(0, 4, 7, "\nPROHRAVAS! Krupier ma vyssi skore!");
		ukHrac->sazka = 0;
	}
}

void PrevedNaKartu(USHORT& pI, stringstream& pProud) {
	string s;

	switch (pI) {
	case 74:
		pProud << "J" << ", ";
		break;
	case 81:
		pProud << "Q" << ", ";
		break;
	case 75:
		pProud << "K" << ", ";
		break;
	case 11:
		pProud << "A" << ", ";
		break;
	default:
		pProud << pI << ", ";
		break;
	}
}

void SkoreSoucet(USHORT& suma, USHORT& pI) {
	if ((pI == 74) || (pI == 81) || (pI == 75)) {
		suma += 10;
	}
	else {
		suma += pI;
	}
}

bool KontrolaPrazdnehoBanku(Krupier *ukKrupier, Hrac *ukHrac) {
	if (ukHrac->SpocitejBank() < 100) {
		char volba;
		system("CLS");

		if (ukHrac->SpocitejBank() == 0) {
			cout << "\Prohral jsi vsechy penize! Hra bude ukoncena.";
			cout << "\nPokracuj stisknutim 1: ";
		}
		else if ((ukHrac->SpocitejBank() > 0)
			&& (ukHrac->SpocitejBank() < 100)) {
			cout << "\Minimalni sazka je 100. Nemas dostatek penez, abys mohl vsadit! Hra bude ukoncena.";
			cout << "\nPokracuj stisknutim 1: ";
		}
		cin >> volba;

		while (volba != '1') {
			cout << "\nZadal jsi spatne cislo, opakuj: ";
			cin >> volba;
		}
		return true;
	}
	return false;
}

// Funkce vypíše barevný text na stdout
void BarevnyVystup(int barvaPozadiPred, int barvaPoprediPred, int puvodniBarva, const string retezec) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		barvaPozadiPred * 16 + barvaPoprediPred);
	cout << retezec;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), puvodniBarva); // Původní barva textu
}
