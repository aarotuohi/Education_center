/* Kurssikeskus
*
* Kuvaus:
*
* Ohjelma lukee ja jasentelee lukemastaan tiedostosta kurssin teeman, kurssin nimen, paikkakunnan
* seka osallistujamaaran.
* Mikali ohjelma tiedostoa ei voida lukea tulostaa ohjelma virheilmoituksen, jos tiedostoa ei loydy.
* Lisaksi jos syotetty tiedosto on tyhja tulostetaan erityisilmoitus.
* Ohjelma tallentaa tiedot maarattyyn tietorakenteeseen map, joka sisaltaa vektorissa olevan struct rakenteen.
* Avaimena toimii kurssin teema.
* Jos tiedoston lukeminen onnistuu tallennetaan tiedot valittuun tietorakenteeseen.
* Taman jalkeen alkaa kayttoliittyma kaymaan haluttuja komentoja lapi loopin avulla.
* Lisaksi ohjelma tarkistaa onko annetut komennot oikea ja tulostaa virheilmoituksen jos komento ei ole haluttu.
* Samoin kuin komennon tarkistaminen myos kayttajan syote tarkistetaan ja tarvittaessa tulostetaan virheilmoitus.
* Komentoja on seitseman erilaista,joilla voit esimerkiksi tarkistaa paikkakuntien kurssitarjonnan tai
* vapaina olevien kurssien paikkakunnan, nimen seka osallistujamaaran.
* Kun komento on syotetty oikein ohjelma tulostaa halutun tiedon.
* Ohjelma toimii kayttoliittyman ymparilla niin kauan kun kayttaja syottaa komennon quit, jolloin ohjelma sulkeutuu
* Funktioissa on luotu kopioita parametrina saadusta tietokannasta, jotta sen jarjestys ei pysyvasti muuttuisi.
*
*
* Ohjelman kirjoittajat
* Nimi: Konsta Tauriainen
* Opiskelijanumero: k430829
* kt430829 https://course-gitlab.tuni.fi/comp.cs.110-ohj-2_2021-SYKSY/kt430829.git
* E-Mail: konsta.tauriainen@tuni.fi
* ja
* Nimi: Aaro Tuohi
* Kayttajatunnus msaatu
* Opiskelijanumero: 050280618
* Sahkoposti: aaro.tuohi@tuni.fi
* Huomioita ohjelmasta ja sen toteutuksesta:
*
* */

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <utility>

using namespace std;
const char CSV_FIELD_DELIMETER = ';';
const int COURSE_FULL = 50;

// Jakaa annetun merkkijonon str annetun merkin delimiter erottelemiin osiin
// ja palauttaa osat vektorissa.
// Jos erotinmerkki on lainausmerkkien sisalla, erotinmerkista ei valiteta,
// eli osaa olla erottelematta lainausmerkkien sisalla olevan tekstin.


// Alustetaan struct-rakenne, jossa on jokaiselle kurssille kolme muuttujaa.
struct Course {
    string name;
    string location;
    int enrollments;

    // Luodaan vertailuoperaattori, jota hy�?dynnet�?�?n j�?rjest�?misen kanssa.
    bool operator<(const Course& a) const {
         return (this->name < a.name);
    }
};

// Luodaan funktio, joka ottaa kaksi merkkijonoparia parametrina,
// ja palauttaa totuusarvon siita onko ensimmainen ennen toista aakkosissa.
// Tata funktiota kaytetaan sort-funktion parametrina myohemmin.
bool compare_location(const pair<string, string>& a, const pair<string, string>& b) {
    return a.first < b.first;
}

// Luodaan funktio, joka vertaa edellisen funktion lisaksi myos kurssien nimia,
// jos paikkakunta on sama.
bool compare_location_and_name(const Course& a, const Course& b) {
    if(a.location == b.location) {
        return a.name < b.name;
    } else {
        return a.location < b.location;
    }
}

// Valmiiksi annettu funktio, jonka avulla pystytaan syottamaan parametrit
// funktioille ilman lainausmerkkeja.
vector<string> split_ignoring_quoted_delim(const string& str, char delimiter) {

    std::vector<std::string> result = {""};
    bool inside_quotation = false;
    for(char current_char : str) {
        if(current_char == '"') {
            inside_quotation = not inside_quotation;
        }
        else if(current_char == delimiter and not inside_quotation) {
            result.push_back("");
        }
        else {
            result.back().push_back(current_char);
        }
    }

    if(result.back() == "") {
        result.erase(--result.end());
    }
    return result;
}

// Funktio lukee tiedoston, ja kertoo paluuarvona, ett�? onnistuuko sen avaus.
bool read_input_file(const string& filepath, vector<string>& lines){
    ifstream input_file_in(filepath);
    if(!input_file_in.is_open()) {
        return false;
    }

    string line;
    while(getline(input_file_in, line)) {
        lines.push_back(line);
    }
    return true;
}

// Funktio kertoo onko tiedostolla luetulla rivilla juuri oikea maara
// syotteita, ja palauttaa totuusarvon.
bool validate_no_empty_fields(const vector<string>& fields){
     if(fields.size() != 4) {
         return false;
     }

     for (const auto& line : fields) {
        if(line.empty()) {
            return false;
        }
    }
    return true;
}

// Funktio tulostaa parametrinaan saaman tietokannan avaimet jarjestyksessa.
void themes_in_order(const map<string, vector<Course>>& kansalaisopisto) {
    for (const auto& themes : kansalaisopisto) {
        if (themes.first != "") {
        cout << themes.first << endl;
        }
    }
}

// Funktio tulostaa kurssit paikkakunnittain. Parametreina otetaan tietokanta,
// paikkakunta seka teema.
void courses_by_location(const map<string, vector<Course>>& education_center,
                         string location, string teema) {

    // Luodaan apumuuttuja tarkistusta varten, etta loytyyko paikkakuntaa.
    bool is_location_found = false;

    // Ellei teemaa loydy tietokannasta, tulostetaan virheviesti.
    if(education_center.find(teema) == education_center.end()) {
        cout << "Error: unknown theme\n";
        return;
    }

    // Alustetaan vektori, johon tallennetaan kurssien tiedot.
    vector<Course> courses_by_location;
    for (const auto& themes : education_center) {
        for (const auto& course_location : education_center.at(themes.first)) {
            if(course_location.location == location) {
            is_location_found = true;

                if (teema == themes.first) {
                courses_by_location.push_back(course_location);
                }
            }
        }
    }

    // Ellei paikkakuntaa loydy, tulostetaan virheviesti.
    if(!is_location_found) {
        cout << "Error: unknown location\n";
        return;
    }

    // Jarjestetaan vektori aakkosjarjestykseen ja tulostetaan sisalto.
    sort(courses_by_location.begin(), courses_by_location.end());
    for (const auto& courses : courses_by_location) {
        if (courses.enrollments == 50) {
            cout << courses.name << " --- " << "full\n";
        } else {
        cout << courses.name << " --- " << courses.enrollments << " enrollments\n";
        }
    }
}

// Luodaan funktio, joka tulostaa kaikki kurssit, joille on viela tilaa.
void available_courses (const map<string, vector<Course>>& education_center) {

    map<string, vector<Course>> education_center_copy = education_center;

    // Jarjestetaan tietokanta ensin paikkakunnan ja sitten nimen
    // mukaan aakkosjarjestykseen.
    for (auto& themes : education_center_copy) {
        sort(themes.second.begin(), themes.second.end(),
             compare_location_and_name);
    }

    // Kaydaan kaikki tietokannan kurssit lapi, ja tulostetaan
    // tiedot kursseista, jotka eivat ole taynna.
    for (const auto& themes : education_center_copy) {
        for (const auto& courses : themes.second) {
            if(courses.enrollments != COURSE_FULL) {
        cout << themes.first << " : " <<
                courses.location << " : " <<
                courses.name << endl;
            }
        }
    }
}



// Luodaan funktio, joka tarkistaa parametrina annetun tietokannan
// suosituimman teeman, ja tulostaa sen tai useamman tarvittaessa.
void favorite_theme (const map<string, vector<Course>>& education_center) {

    // Jos tietokanta on tyhja, tulostetaan viesti kayttajalle.
    // Muussa tapauksessa, lisataan apuvektoriin suosituimman teeman nimi
    // seka sen osallistujamaara.
    if(education_center.empty()) {
        cout << "No enrollments" << endl;
    } else {

        vector<pair <string, int>> favorite_themes;
        favorite_themes.push_back(make_pair("", 0));

        // Kaydaan lapi kaikki kurssit koko tietokannasta, ja lisataan
        // kurssin nimi vektoriin, jos se on suositumpi kuin siell�? oleva
        // kurssi. Jos kurssit ovat yhta suosittuja, ei vektorin tyhjennysta
        // suoriteta ollenkaan.
        for (const auto& themes : education_center) {
            int enrollments = 0;
            for (const auto& courses : themes.second) {
                enrollments += courses.enrollments;
            }

            if (enrollments > favorite_themes.at(0).second) {
                favorite_themes.clear();
                favorite_themes.push_back(make_pair(themes.first, enrollments));
            }

            else if (enrollments == favorite_themes.at(0).second) {
                favorite_themes.push_back(make_pair(themes.first, enrollments));
            }
        }

        cout << favorite_themes.at(0).second << " enrollments in themes\n";
        for (const auto& favorites : favorite_themes) {
            cout << "--- " << favorites.first << endl;
        }
    }
}


void courses_in_location(const map<string,vector<Course>>& education_center,
                         const string location ) {

    // Alustetaan apumuuttuja siita, ett�? loytyyko kurssia ollenkaan
    bool is_course_found = false;

    map<string,vector<Course>> education_center_copy = education_center;

    // Kaydaan lapi kaikki mapista loytyvat kurssit, ja tarkistetaan,
    // etta loytyyko kyseista kurssia ollenkaan.
    for (const auto& themes : education_center_copy) {
        for (const auto& course : education_center_copy.at(themes.first)) {
            if(course.location == location) {
            is_course_found = true;
            }
        }
    }

    // Kurssin loydyttya, suoritetaan niiden lisaaminen vektoriin.
    // Muussa tapauksessa tulostetaan kayttajalle virheviesti.
    if (is_course_found) {
        // Luodaan apumuuttujavektori kurssin paikkakunnille.
        vector<string> courses_in_location;

        // Jos kurssin paikkakunta tasmaa annettuun parametriin, lisataan
        // kurssin nimi apuvektoriin.
        for(const auto& themes : education_center_copy){
            for (const auto& course : education_center_copy.at(themes.first)) {
            if(course.location == location) {
               courses_in_location.push_back(course.name);
               }
            }
        }

        // Jarjestetaan kurssit paikkakunnittain aakkojarjestykseen seka
        // tulostetaan ne.
        sort(courses_in_location.begin(),courses_in_location.end());
        for(const auto& course : courses_in_location){
            cout << course << endl;
        }
    } else {
        cout << "Error: unknown location\n";
    }
}


// Luodaan funktio, joka tulostaa kaikki teeman kurssit paikkakunnan mukaan.
void courses_in_themes(const map<string,vector<Course>>& education_center,
                       const string theme ) {

    map<string,vector<Course>> education_center_copy = education_center;

    // Tarkistetaan loytyyko parametrina saatua teemaa tietokannasta,
    // ja tulostetaan virheviesti, ellei sita loydy.
    if(education_center_copy.find(theme) == education_center_copy.end()) {
        cout << "Error: unknown theme\n";
        return;
    }

    // Luodaan apuuttujaksi tietopari, jossa kaksi merkkijonoa.
    vector<pair <string, string>> courses_in_themes;
    courses_in_themes.push_back(make_pair("", ""));

    // Kaydaan lapi teemat seka kurssit ja lisataan ne apumuuttujaan,
    // jotta niiden jarjestaminen onnistuu myohemmin helpommin.
    for (const auto& themes : education_center_copy) {
        if (theme == themes.first) {
        for(const auto& course : themes.second){
            courses_in_themes.push_back(make_pair(course.location, course.name));
            }
        }
    }

    // Jarjestetaan kurssit paikkakunnittain, siten etta kurssien nimet
    // sailyy alkuperaisjarjestyksessa.
    sort(courses_in_themes.begin(),courses_in_themes.end(), compare_location);
    for(const auto& course : courses_in_themes) {
        if(course.first != "" and course.second != "") {
            cout << course.first << " : " << course.second << endl;
        }
    }
}


// Luodaan funktio, joka peruuttaa parametrina annetun kurssin
// kaikilta paikkakunnilta.
void cancel(map<string, vector<Course>>& education_center,
            string canceled_course) {

    // Alustetaan apumuuttuja siita, ett�? loytyyko kurssia ollenkaan
    bool is_course_found = false;

    // Kaydaan lapi kaikki mapista loytyvat kurssit, ja tarkistetaan,
    // etta loytyyko kyseista kurssia ollenkaan.
    for (const auto& themes : education_center) {
        for (const auto& course : education_center.at(themes.first)) {
            if(course.name == canceled_course) {
            is_course_found = true;
            }
        }
    }

    // kun ollaan varmoja, etta kurssi loytyy, poistetaan kurssi kaikista
    // teemoista, poistetaan se.
    if (is_course_found) {
        for (auto& theme : education_center ) {
            if (theme.second.erase(remove_if(theme.second.begin(),
                                             theme.second.end(),
        [&] (Course const kurssi) {return kurssi.name == canceled_course; }),
        theme.second.end()) != theme.second.end()) {
        }
    }
        // Tulostetaan kayttajalle viesti poiston onnistumisesta.
        // Muuten tulostetaan viesti, ettei kurssia ole loytynyt.
        cout << canceled_course << " cancelled in all locations" << endl;
    } else {
        cout << "Error: unknown course" << endl;
    }
}

void user_interface(map<string, vector<Course>>& education_center) {
    for(;;) {

        // Otetaan talteen kayttajan antama(t) merkkijono(t).
        string input;
        cout << "> ";
        getline(cin, input);

        // Tallennetaan merkkijono(t) vektoriin. Apuna kaytetaan valmiiksi
        // annettua funktiota, joka huomioi mahdolliset moniosaiset komennot.
        vector<string> user_inputs = split_ignoring_quoted_delim(input , ' ');
        string command = user_inputs.at(0);


        // Jos lopetuskomento syotetaan, palataan main-funktioon.
        if(command == "quit") {
            break;
        }


        // Kutsutaan teemat tulostavaa funktiota.
        else if(command == "themes") {
            themes_in_order(education_center);
        }


        // Kutsutaan kurssit paikkakunnittain tulostavaa funktiota,
        // jos parametreja on oikea maara. Muutoin tulostetaan virheviesti.
        else if(command == "courses") {
            if(user_inputs.size() == 3) {
                courses_by_location(education_center,
                                    user_inputs.at(1), user_inputs.at(2));
            } else {
                cout << "Error: error in command " << command << endl;
            }
         }


        //Kutsutaan kurssit, joissa viela tilaa osallistujille.
        else if(command == "available") {
            available_courses(education_center);
        }


        // Kutsutaan funktiota, joka tulostaa suosituimman teeman.
        else if(command == "favorite_theme") {
            favorite_theme(education_center);
        }


        // Kutsutaan paikkakunnan kaikki kurssit tulostavaa funktiota, jos
        // parametrin maara on oikea. Muutoin tulostetaan virheilmoitus.
        else if(command == "courses_in_location") {
            if(user_inputs.size() == 2) {
                courses_in_location(education_center, user_inputs.at(1));
            } else {
                cout << "Error: error in command " << command << endl;
            }
        }


        // Kutsutaan funktiota, joka tulostaa teeman kaikki kurssit, jos
        // parametreja on oikea maara. Muutoin tulostetaan virheilmoitus.
        else if(command == "courses_in_theme") {
            if(user_inputs.size() == 2) {
                courses_in_themes(education_center, user_inputs.at(1));
            } else {
                cout << "Error: error in command " << command << endl;
            }
        }


        else if(command == "cancel") {
            cancel(education_center, user_inputs.at(1));
        }


        // Jos kayttajan syote ei ole mikaan tunnettu komento,
        // tulostetaan virheviesti.
        else {
            cout << "Error: Unknown command: " << command << endl;
        }
    }
}


// Luodaan funktio, joka tarkistaa rivien oikeellisuuden, ja lisaa ne
// tietorakenteeseen, seka palauttaa totuusarvon taman onnistumisesta.
bool parse_input_lines(const vector<string>& lines,
                       map<string,vector<Course>>& education_center) {

    // Kaydaan lapi parametrina saadun vektorin rivit yksi kerrallaan.
    for(const auto& line : lines) {
        // Jaetaan valmiiksi annettua funktiota apuna kayttaen rivin data
        // omiin kenttiinsa, ja tarkastetaan, ettei mukana ole
        // tyhjia riveja.
        vector<string> fields = split_ignoring_quoted_delim
        (line, CSV_FIELD_DELIMETER);

        // Jos tiedostossa on ollut tyhjia riveja, palataan mainiin.
        if(!validate_no_empty_fields(fields)) {
            return false;
        }

        // Alustetaan muuttujat kenttien arvoilla.
        const string theme = fields.at(0);
        const string& name = fields.at(1);
        const string& location = fields.at(2);
        int enrollments = 0;

        // Jos syotetiedostossa on merkitty kurssin olevan taynna,
        // muutetaan osallistujamaara maksimiin eli vakiona annettuun 50.
        // Muussa tapauksessa lisataan syotteena ollu luku.
        if (fields.at(3) == "full") {
            enrollments = COURSE_FULL;
        } else {
            istringstream course_enrollments_in(fields.at(3));
            if(!(course_enrollments_in >> enrollments)){
                return false;
            }
        }

        // Luodaan uusi struct oikeaksi jo varmistetuilla muuttujilla.
        Course new_course = Course{name, location, enrollments};

        education_center.try_emplace({});

        // Luodaan vektori kursseista, jotta se voidaan myohemmin tallentaa
        // mappiin valuena.
        vector<Course>& courses_under_theme = education_center[theme];

        auto it = courses_under_theme.begin();

        // Jos kyseinen kurssi loytyy jo samalta paikkakunnalta,
        // Lisataan vain osallistujamaara uutena arvona.
        for(; it != courses_under_theme.end() ; it++) {
            if(it->name == new_course.name && it->location == new_course.location) {
                break;
            }
        }

        if (it != courses_under_theme.end()) {
            courses_under_theme.erase(it);
        }
        // Lisataan uusi kurssi teeman alla olevien kurssien vektoriin.
        courses_under_theme.push_back(new_course);
    }
    return true;
}

int main() {
    string input_file;
    cout << "Input file: ";
    getline(cin, input_file);

    vector<string> input_file_lines;

    // Tarkistetaan onnistuuko tiedoston avaus. Tulostetaan
    // virheilmoitus, jos ei saada tiedostoa auki.
    if(!read_input_file(input_file, input_file_lines)){
        cerr << "Error: the input file cannot be opened\n";
        return EXIT_FAILURE;
    }

    // Luodaan koko ohjelmassa kaytettava tietorakenne.
    map<string, vector<Course>> education_center;

    // Annetaan tietorakenne viiteparametrina rivien oikeellisuuden
    // tarkistavaan funktioon.
    if(!parse_input_lines(input_file_lines,education_center)) {
        cerr << "Error: empty field\n";
        return EXIT_FAILURE;
    }

    // Kun tiedosto on saatu auki ja sen oikeellisuus on tarkistettu,
    // kutsutaan ohjelman toimintaa pyorittavaa kayttoliittymaa.
    else {
         user_interface(education_center);
    }
    return EXIT_SUCCESS;
}
