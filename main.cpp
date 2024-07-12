#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <iomanip>
#include <sstream>

using namespace std;

// Algoritmo O(n^3) Enfoque recursivo
bool isPalindrome(const string& str, int i, int j) {
    while (i < j) {
        if (str[i] != str[j])
            return false;
        i++;
        j--;
    }
    return true;
}

int minCutsRecursive(const string& str, int i, int j, vector<vector<int>>& dp) {
    if (i >= j || isPalindrome(str, i, j))
        return 0;

    if (dp[i][j] != -1)
        return dp[i][j];

    int minCuts = INT_MAX;
    for (int k = i; k < j; k++) {
        if (isPalindrome(str, i, k)) {
            int cuts = 1 + minCutsRecursive(str, k + 1, j, dp);
            minCuts = min(minCuts, cuts);
        }
    }

    return dp[i][j] = minCuts;
}

int minCuts(const string& str) {
    int n = str.size();
    vector<vector<int>> dp(n, vector<int>(n, -1));
    return minCutsRecursive(str, 0, n - 1, dp);
}

// Algoritmo O(n^2) Enfoque de programacion dinamica
int minCutsDP(const string& str) {
    int n = str.size();
    vector<vector<bool>> P(n, vector<bool>(n, false));
    vector<int> C(n, 0);

    for (int i = 0; i < n; i++) {
        P[i][i] = true;
    }

    for (int L = 2; L <= n; L++) {
        for (int i = 0; i < n - L + 1; i++) {
            int j = i + L - 1;
            if (L == 2)
                P[i][j] = (str[i] == str[j]);
            else
                P[i][j] = (str[i] == str[j]) && P[i + 1][j - 1];
        }
    }

    for (int i = 0; i < n; i++) {
        if (P[0][i] == true) {
            C[i] = 0;
        }
        else {
            C[i] = INT_MAX;
            for (int j = 0; j < i; j++) {
                if (P[j + 1][i] == true && 1 + C[j] < C[i]) {
                    C[i] = 1 + C[j];
                }
            }
        }
    }

    return C[n - 1];
}

// Generador de palabras aleatorias en archivo
void GenerarArchivoPalabras() {
    ofstream Palabras("Palabras.txt", ios::app);
    if (!Palabras) {
        cerr << "Error al abrir el archivo Palabras" << endl;
        return;
    }

    const char caracteres[] = "abc";
    const int numCaracteres = sizeof(caracteres) - 1;
    int longitud = 0;

    srand(static_cast<unsigned int>(time(0)));
    for (int i = 1; i <= 2000; ++i) {
        longitud = rand() % 2000 + 1;
        string linea;
        for (int j = 0; j < longitud; ++j) {
            linea += caracteres[rand() % numCaracteres];
        }
        Palabras << linea << endl;
    }

    Palabras.close();
}

// Leer los datos de tiempos.csv y almacenarlos en vectores
void LeerDatos(vector<float>& longitudes, vector<float>& tiemposRecursivo, vector<float>& tiemposDP) {
    ifstream tiempos("tiempos.csv");
    if (!tiempos) {
        cerr << "Error al abrir el archivo tiempos.csv" << endl;
        return;
    }

    string linea;
    while (getline(tiempos, linea)) {
        size_t pos1 = linea.find(",");
        size_t pos2 = linea.find(",", pos1 + 1);

        float longitud = stof(linea.substr(0, pos1));
        float tiempoRecursivo = stof(linea.substr(pos1 + 1, pos2 - pos1 - 1));
        float tiempoDP = stof(linea.substr(pos2 + 1));

        longitudes.push_back(longitud);
        tiemposRecursivo.push_back(tiempoRecursivo);
        tiemposDP.push_back(tiempoDP);
    }

    tiempos.close();
}

// Dibujar el gráfico de dispersión
void DibujarGrafico(const vector<float>& longitudes, const vector<float>& tiemposRecursivo, const vector<float>& tiemposDP) {
    sf::RenderWindow window(sf::VideoMode(800, 700), "Gráfico de Dispersión | En rojo recursivo | En azul dinamico");

    // Cargar una fuente
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cerr << "Error al cargar la fuente" << endl;
        return;
    }
    

    // Escala máxima para los ejes X e Y
    float maxLongitud = *max_element(longitudes.begin(), longitudes.end());
    float maxYRecursivo = *max_element(tiemposRecursivo.begin(), tiemposRecursivo.end());
    float maxYDP = *max_element(tiemposDP.begin(), tiemposDP.end());
    float maxY = max(maxYRecursivo, maxYDP); // Tomar el mayor de los dos para el eje Y

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        // Dibujar ejes X e Y
        sf::Vertex xAxis[] = {
            sf::Vertex(sf::Vector2f(50, 550), sf::Color::Black),
            sf::Vertex(sf::Vector2f(750, 550), sf::Color::Black)
        };

        sf::Vertex yAxis[] = {
            sf::Vertex(sf::Vector2f(50, 550), sf::Color::Black),
            sf::Vertex(sf::Vector2f(50, 50), sf::Color::Black)
        };

        window.draw(xAxis, 2, sf::Lines);
        window.draw(yAxis, 2, sf::Lines);

        // Dibujar puntos para el tiempo recursivo
        sf::CircleShape pointRecursivo(2);
        pointRecursivo.setFillColor(sf::Color::Red);

        for (size_t i = 0; i < longitudes.size(); ++i) {
            float x = 50 + (longitudes[i] / maxLongitud) * 700; // Escala en el eje X
            float y = 550 - (tiemposRecursivo[i] / maxYRecursivo) * 500; // Escala en el eje Y

            pointRecursivo.setPosition(x, y);
            window.draw(pointRecursivo);
        }

        // Dibujar puntos para el tiempo DP
        sf::CircleShape pointDP(2);
        pointDP.setFillColor(sf::Color::Blue);

        for (size_t i = 0; i < longitudes.size(); ++i) {
            float x = 50 + (longitudes[i] / maxLongitud) * 700; // Escala en el eje X
            float y = 550 - (tiemposDP[i] / maxYDP) * 500; // Escala en el eje Y

            pointDP.setPosition(x, y);
            window.draw(pointDP);
        }
        // Dibujar etiquetas en el eje X
        for (int i = 0; i <= 10; ++i) {
            float x = 50 + (i / 10.0) * 700;
            sf::Text label(to_string(static_cast<int>(i * maxLongitud / 10)), font, 12);
            label.setFillColor(sf::Color::Black);
            label.setPosition(x, 555);
            window.draw(label);
        }

        // Dibujar etiquetas en el eje Y con 4 decimales
        for (int i = 0; i <= 10; ++i) {
            float y = 550 - (i / 10.0) * 500;

            // Usar ostringstream para controlar la precisión
            ostringstream out;
            out << fixed << setprecision(3) << (i * maxY / 10.0);

            sf::Text label(out.str(), font, 12);
            label.setFillColor(sf::Color::Black);
            label.setPosition(15, y - 10);
            window.draw(label);
        }
        window.display();
    }
}


int main() {
    GenerarArchivoPalabras();

    ofstream tiempos("tiempos.csv", ios::app);
    ifstream Palabras("Palabras.txt", ios::app);
    int i = 1;
    string linea;

    if (!tiempos) {
        cerr << "Error al abrir el archivo 'tiempos'" << endl;
        return 0;
    }
    if (!Palabras) {
        cerr << "Error al abrir el archivo 'Palabras'" << endl;
        return 0;
    }

    while (getline(Palabras, linea)) {
        cout << i << endl;
        size_t longitud = linea.length();

        auto inicioMincut = chrono::high_resolution_clock::now();
        minCuts(linea);
        auto finMincut = chrono::high_resolution_clock::now();
        chrono::duration<double> duracionMincut = finMincut - inicioMincut;

        auto inicioMincutSDP = chrono::high_resolution_clock::now();
        minCutsDP(linea);
        auto finMincutSDP = chrono::high_resolution_clock::now();
        chrono::duration<double> duracionMincutSDP = finMincutSDP - inicioMincutSDP;

        tiempos << longitud << "," << duracionMincut.count() << "," << duracionMincutSDP.count() << "\n";
        ++i;
    }

    Palabras.close();
    tiempos.close();

    vector<float> longitudes;
    vector<float> tiemposRecursivo;
    vector<float> tiemposDP;

    LeerDatos(longitudes, tiemposRecursivo, tiemposDP);
    DibujarGrafico(longitudes, tiemposRecursivo, tiemposDP);
    cout << "Presione enter para finalizar..." << endl;
    cin.get();
    return 0;
}
