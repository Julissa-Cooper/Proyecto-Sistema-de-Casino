#include <iostream>
//Manejo de archivos
#include <fstream>
//Manipuladores de formato de salida para los numeros
#include <iomanip>
//La clase String
#include <string>
//Funciones generales de C (rand, srand, system)
#include <cstdlib>
//Manejo de tiempo/fechas (para la semilla de rand)
#include <ctime>
//Funciones matematicas
#include <cmath>
//Para formatear numeros a texto
#include <cstdio>
#include <windows.h>

using namespace std;

//Constantes globales
//Archivo pa' guardar historial de apuestas de la sesion
const string ARCHIVO_HISTORIAL = "historial_casino.txt";
//Archivo de la tabla de clasificacion 
const string ARCHIVO_CLASIFICACION = "clasificacion.txt";
//Maximo de registros que el arreglo de historial en memoria puede guardar
const int MAX_HIST = 100;
//Maximo de registros que se pueden leer desde clasificacion.txt
const int MAX_RANKING = 500;

//Colores ANSI
const string RESET      = "\033[0m";
const string NEGRITA    = "\033[1m";
const string ROJO       = "\033[31m";
const string VERDE      = "\033[32m";
const string AMARILLO   = "\033[33m";
const string CIAN       = "\033[36m";
const string ROJO_INT   = "\033[91m";
const string VERDE_INT  = "\033[92m";
const string DORADO     = "\033[93m";
const string MAGENTA_INT = "\033[95m";
const string CIAN_INT   = "\033[96m";
const string ROSA       = "\033[38;5;198m";

//Funciones para efectos en pantalla / estetica

//Limpiar la consola
void limpiarPantalla() {
    system("cls");
}

//Pausa el programa hasta que el usuario presione enter
void pausar() {
    cout << "\n" << CIAN << NEGRITA << ">> Presiona ENTER para continuar..." << RESET;
    cin.ignore(1000, '\n'); //limpia el buffer sin necesitar <limits>
    cin.get();
}

//FUNCIONES AUXILIARES

//Convierte un monto a texto con 2 decimales fijos 
string formatoMonto(double monto) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f", monto);
    return string(buffer);
}

//Imprime un monto (ganado o perdido) resaltado dentro de una caja de caracteres
void resaltarMonto(const string &etiqueta, double monto, bool esGanancia) {
    string colorMarco, colorMonto, signo;
    if (esGanancia) {
        colorMarco = ROSA;
        colorMonto = DORADO;
        signo = "+$";
    } else {
        colorMarco = ROJO_INT;
        colorMonto = ROJO_INT;
        signo = "-$";
    }

    string texto = " " + etiqueta + "  " + signo + formatoMonto(monto) + " ";
    string borde(texto.length() + 4, '=');

    cout << "\n" << colorMarco << NEGRITA << "   " << borde << RESET << endl;
    cout << colorMarco << NEGRITA << "   ||" << RESET << colorMonto << NEGRITA
         << texto << RESET << colorMarco << NEGRITA << "||" << RESET << endl;
    cout << colorMarco << NEGRITA << "   " << borde << RESET << endl;
}

//Agrega una linea al archivo de historial con nombre, juego, apuesta, resultado y saldo
void registrarApuestaArchivo(const string &nombre, const string &juego, double apuesta, const string &resultado, double saldoActual) {
    ofstream archivo(ARCHIVO_HISTORIAL, ios::app);
    if (archivo.is_open()) {
        archivo << nombre << ";" << juego << ";" << formatoMonto(apuesta) << ";" << resultado << ";" << formatoMonto(saldoActual) << "\n";
        archivo.close();
    } else {
        cout << ROJO << "Aviso: no se pudo abrir " << ARCHIVO_HISTORIAL << RESET << endl;
    }
}

//Agrega el resumen de una partida terminada a la tabla de clasificacion
void guardarResumenClasificacion(const string &nombre, int partidasJugadas, double saldoInicial, double saldoFinal) {
    ofstream archivo(ARCHIVO_CLASIFICACION, ios::app);
    if (archivo.is_open()) {
        double ganancia = saldoFinal - saldoInicial;
        archivo << nombre << ";" << partidasJugadas << ";" << formatoMonto(saldoInicial) << ";"
                << formatoMonto(saldoFinal) << ";" << formatoMonto(ganancia) << "\n";
        archivo.close();
    } else {
        cout << ROJO << "Aviso: no se pudo guardar el resumen en " << ARCHIVO_CLASIFICACION << RESET << endl;
    }
}

//Carga la clasificacion guardada en clasificacion.txt hacia los arreglos.
int cargarClasificacion(string nombres[], int partidas[], double saldoIniciales[], double saldoFinales[], double ganancias[]) {
    ifstream archivo(ARCHIVO_CLASIFICACION);
    int cantidad = 0;

    if (archivo.is_open()) {
        string linea;
        while (getline(archivo, linea) && cantidad < MAX_RANKING) {
            if (linea.empty()) continue;

            //Se buscan las posiciones de los ';' para separar los 5 campos
            int pos1 = linea.find(';');
            int pos2 = linea.find(';', pos1 + 1);
            int pos3 = linea.find(';', pos2 + 1);
            int pos4 = linea.find(';', pos3 + 1);

            nombres[cantidad]         = linea.substr(0, pos1);
            partidas[cantidad]        = stoi(linea.substr(pos1 + 1, pos2 - pos1 - 1));
            saldoIniciales[cantidad]  = stod(linea.substr(pos2 + 1, pos3 - pos2 - 1));
            saldoFinales[cantidad]    = stod(linea.substr(pos3 + 1, pos4 - pos3 - 1));
            ganancias[cantidad]       = stod(linea.substr(pos4 + 1));

            cantidad++;
        }
        archivo.close();
    }
    //Si el archivo no existe todavia, regresa 0
    return cantidad;
}

//Ordena y muestra la tabla de clasificacion de mayor a menor ganancia
void mostrarTablaClasificacion() {
    string nombres[MAX_RANKING];
    int partidas[MAX_RANKING];
    double saldoIniciales[MAX_RANKING], saldoFinales[MAX_RANKING], ganancias[MAX_RANKING];

    int cantidad = cargarClasificacion(nombres, partidas, saldoIniciales, saldoFinales, ganancias);

    cout << ROSA << NEGRITA << "\n================ TABLA DE CLASIFICACION ================" << RESET << endl;

    if (cantidad == 0) {
        cout << CIAN << "\nAun no hay partidas registradas en la tabla de clasificacion." << RESET << endl;
        return;
    }

    //Ordena por ganancia neta descendente
    for (int i = 0; i < cantidad - 1; i++) {
        for (int j = 0; j < cantidad - 1 - i; j++) {
            if (ganancias[j] < ganancias[j + 1]) {
                string tempNombre = nombres[j];
                nombres[j] = nombres[j + 1];
                nombres[j + 1] = tempNombre;

                int tempPartidas = partidas[j];
                partidas[j] = partidas[j + 1];
                partidas[j + 1] = tempPartidas;

                double tempSI = saldoIniciales[j];
                saldoIniciales[j] = saldoIniciales[j + 1];
                saldoIniciales[j + 1] = tempSI;

                double tempSF = saldoFinales[j];
                saldoFinales[j] = saldoFinales[j + 1];
                saldoFinales[j + 1] = tempSF;

                double tempG = ganancias[j];
                ganancias[j] = ganancias[j + 1];
                ganancias[j + 1] = tempG;
            }
        }
    }

    //encabezado
    cout << left << setw(4) << "#" << setw(14) << "Jugador" << setw(10) << "Partidas"
         << setw(15) << "Saldo Inicial" << setw(15) << "Saldo Final" << "Ganancia" << endl;
    cout << "----------------------------------------------------------" << endl;

    for (int i = 0; i < cantidad; i++) {
        string colorGanancia, signoGanancia;
        if (ganancias[i] >= 0) {
            colorGanancia = VERDE_INT;
            signoGanancia = "+$";
        } else {
            colorGanancia = ROJO_INT;
            signoGanancia = "-$";
        }

        cout << left << setw(4) << (i + 1) << setw(14) << nombres[i] << setw(10) << partidas[i]
             << "$" << setw(14) << formatoMonto(saldoIniciales[i])
             << "$" << setw(14) << formatoMonto(saldoFinales[i])
             << colorGanancia << signoGanancia << formatoMonto(fabs(ganancias[i])) << RESET << endl;
    }
    cout << ROSA << NEGRITA << "==========================================================" << RESET << endl;
}

//Se muestra el arreglo de historial completo de la sesion
void mostrarHistorialCompleto(const string &nombre, string histJuego[], double histMonto[], string histResultado[], double histSaldoDespues[], int histCount) {
    cout << ROSA << NEGRITA << "\n--- Historial completo de " << nombre << " ---" << RESET << endl;
    for (int i = 0; i < histCount; i++) {
        cout << (i + 1) << ". " << histJuego[i];
        if (histMonto[i] > 0) {
            cout << " | Apuesta: $" << formatoMonto(histMonto[i]);
        }
        cout << " | " << histResultado[i] << " | Saldo: $" << formatoMonto(histSaldoDespues[i]) << endl;
    }
}

//Se muestra solo la ultima apuesta registrada
void mostrarUltimaApuesta(string histJuego[], double histMonto[], string histResultado[], int histCount) {
    if (histCount <= 1) {
        cout << CIAN << "Aun no has realizado ninguna apuesta." << RESET << endl;
        return;
    }
    int ultimo = histCount - 1;
    cout << DORADO << NEGRITA << "--- Ultima apuesta ---" << RESET << endl;
    cout << "Juego: " << histJuego[ultimo] << endl;
    cout << "Monto apostado: $" << formatoMonto(histMonto[ultimo]) << endl;
    cout << "Resultado: " << histResultado[ultimo] << endl;
}

//Pantalla de bienvenida
void mostrarBienvenida() {
    string borde(46, '=');
    cout << ROSA << NEGRITA << "   " << borde << RESET << endl;
    cout << ROSA << NEGRITA << "   ||" << string(42, ' ') << "||" << RESET << endl;
    cout << ROSA << NEGRITA << "   ||" << string(16, ' ') << NEGRITA << ROJO_INT << "C A S I N O" << ROSA << string(15, ' ') << "||" << RESET << endl;
    cout << ROSA << NEGRITA << "   ||" << string(42, ' ') << "||" << RESET << endl;
    cout << ROSA << NEGRITA << "   " << borde << RESET << endl;
}

//Muestra saldo final y mensajito de despedida
void mostrarDespedida(const string &nombre, double saldo) {
    cout << DORADO << NEGRITA << "\nSaldo final de " << nombre << ": $" << formatoMonto(saldo) << RESET << endl;
    cout << VERDE_INT << "Gracias por jugar, " << nombre << "! Hasta luego." << RESET << endl;
}

//Funciones del flujo del gameplay

// 00 - Identificacion del usuario y control de acceso: pide nombre y edad
bool validarAcceso(string &nombre) {
    int edad;

    //limpia el buffer antes de getline, esto esa acá para evitar cosas raras con la entrada.
    cin.ignore(1000, '\n'); 
    cout << CIAN << "\nIngrese nombre del jugador: " << RESET;
    getline(cin, nombre);

    cout << CIAN << "Ingrese su edad: " << RESET;
    cin >> edad;

    //Si es de la choza de los pequeñines enontces fuera, sino, entonces dentro
    if (edad >= 18) {
        cout << VERDE_INT << "Bienvenido, " << nombre << "!" << RESET << endl;
        return true;
    } else {
        cout << ROJO_INT << "Acceso denegado, " << nombre << " eres menor de edad" << RESET << endl;
        pausar();
        return false;
    }
}

// 01 - Ingreso y validacion de saldo inicial
double leerSaldoInicial() {
    double saldo;
    do {
        cout << DORADO << "Ingrese saldo inicial: $" << RESET;
        cin >> saldo;
        if (saldo <= 0) cout << ROJO << "Error: Saldo invalido" << RESET << endl;
    } while (saldo <= 0);
    return saldo;
}

// 02 - Menu principal
int leerOpcionMenuPrincipal(const string &nombre, double saldo) {
    int opcion;
    cout << ROSA << NEGRITA << "\n===== MENU PRINCIPAL - " << nombre << " =====" << RESET << endl;
    cout << VERDE_INT << "Saldo: $" << formatoMonto(saldo) << RESET << endl;
    cout << "1. Ver saldo" << endl;
    cout << "2. Ver apuesta anterior" << endl;
    cout << "3. Jugar" << endl;
    cout << "4. Retirarse" << endl;
    cout << CIAN << "Elija una opcion (1-4): " << RESET;
    cin >> opcion;
    return opcion;
}

// 03 - Menu de juegos
int leerOpcionMenuJuegos(const string &nombre, double saldo) {
    int opcion;
    cout << ROSA << NEGRITA << "\n===== MENU DE JUEGOS - " << nombre << " =====" << RESET << endl;
    cout << VERDE_INT << "Saldo actual: $" << formatoMonto(saldo) << RESET << endl;
    cout << "1. Regresar al menu principal" << endl;
    cout << "2. Ruleta" << endl;
    cout << "3. Tragamonedas" << endl;
    cout << "4. Dados" << endl;
    cout << CIAN << "Elija una opcion (1-4): " << RESET;
    cin >> opcion;
    return opcion;
}

//Se pide cuanto se va a apostar 
double pedirApuesta(double saldo) {
    double apuesta;
    do {
        cout << DORADO << "Ingrese monto a apostar: $" << RESET;
        cin >> apuesta;
        if (!(apuesta > 0 && apuesta <= saldo)) {
            cout << ROJO << "Monto invalido, debe ser mayor a 0 y menor o igual al saldo." << RESET << endl;
        }
    } while (!(apuesta > 0 && apuesta <= saldo));
    return apuesta;
}

//Jueguitos: cada uno modifica el saldo (por referencia) y devuelve el texto de resultado

string jugarRuleta(double &saldo, double apuesta) {
    cout << MAGENTA_INT << NEGRITA << "\n--- RULETA ---" << RESET << endl;

    string colores[3] = {"Rojo", "Negro", "Verde"};
    cout << "Colores disponibles:" << endl;
    for (int i = 0; i < 3; i++) {
        cout << (i + 1) << ". " << colores[i] << endl;
    }

    int eleccion;
    do {
        cout << CIAN << "Elija un color (1-3): " << RESET;
        cin >> eleccion;
    } while (eleccion < 1 || eleccion > 3);

    cout << AMARILLO << "Girando la ruleta..." << RESET << endl;

    // 0-36
    int numero = rand() % 37;
    string colorSalido;
    if (numero == 0) colorSalido = "Verde";
    else if (numero % 2 == 0) colorSalido = "Negro";
    else colorSalido = "Rojo";

    cout << AMARILLO << "La ruleta cayo en: " << numero << " (" << colorSalido << ")" << RESET << endl;

    string resultado;
    if (colores[eleccion - 1] == colorSalido) {
        double multiplicador;
        if (colorSalido == "Verde") multiplicador = 14.0;
        else multiplicador = 2.0;

        double premio = apuesta * multiplicador;
        saldo += premio;
        resaltarMonto("GANASTE en Ruleta", premio, true);
        resultado = "Gano $" + formatoMonto(premio) + " en Ruleta";
    } else {
        saldo -= apuesta;
        resaltarMonto("PERDISTE en Ruleta", apuesta, false);
        resultado = "Perdio $" + formatoMonto(apuesta) + " en Ruleta";
    }
    return resultado;
}

string jugarTragamonedas(double &saldo, double apuesta) {
    cout << MAGENTA_INT << NEGRITA << "\n--- TRAGAMONEDAS ---" << RESET << endl;

    int carrete[3];
    cout << AMARILLO << "Girando: " << RESET;
    for (int i = 0; i < 3; i++) {
        // 0-4
        carrete[i] = rand() % 5;
        cout << CIAN_INT << "[" << carrete[i] << "] " << RESET;
    }
    cout << endl;

    string resultado;
    if (carrete[0] == carrete[1] && carrete[1] == carrete[2]) {
        double premio = apuesta * 5.0;
        saldo += premio;
        resaltarMonto("JACKPOT en Tragamonedas", premio, true);
        resultado = "JACKPOT! Gano $" + formatoMonto(premio);
    } else if (carrete[0] == carrete[1] || carrete[1] == carrete[2] || carrete[0] == carrete[2]) {
        cout << AMARILLO << "Dos simbolos iguales: empate, saldo sin cambio." << RESET << endl;
        resultado = "Empate: saldo sin cambio";
    } else {
        saldo -= apuesta;
        resaltarMonto("PERDISTE en Tragamonedas", apuesta, false);
        resultado = "Perdio $" + formatoMonto(apuesta) + " en Tragamonedas";
    }
    return resultado;
}

string jugarDados(double &saldo, double apuesta) {
    cout << MAGENTA_INT << NEGRITA << "\n--- DADOS ---" << RESET << endl;
    cout << "Etapa: 1. ALTA (8-12)   2. BAJA (2-6)" << endl;

    int eleccionDado;
    do {
        cout << CIAN << "Elija opcion (1-2): " << RESET;
        cin >> eleccionDado;
    } while (eleccionDado != 1 && eleccionDado != 2);

    cout << AMARILLO << "Lanzando los dados..." << RESET << endl;

    int d1 = rand() % 6 + 1;
    int d2 = rand() % 6 + 1;
    int suma = d1 + d2;
    cout << AMARILLO << "Dados: " << d1 << " y " << d2 << "  => suma = " << suma << RESET << endl;

    string resultado;
    if (suma == 7) {
        cout << ROJO_INT << NEGRITA << "La casa gana con 7." << RESET << endl;
        saldo -= apuesta;
        resaltarMonto("La casa gano con 7 - PERDISTE", apuesta, false);
        resultado = "Perdio $" + formatoMonto(apuesta) + " en Dados (la casa gano con 7)";
    } else {
        bool prediccionCorrecta = false;
        if (eleccionDado == 1 && suma >= 8 && suma <= 12) prediccionCorrecta = true;
        if (eleccionDado == 2 && suma >= 2 && suma <= 6) prediccionCorrecta = true;

        if (prediccionCorrecta) {
            saldo += apuesta;
            resaltarMonto("GANASTE en Dados", apuesta, true);
            resultado = "Gano $" + formatoMonto(apuesta) + " en Dados";
        } else {
            saldo -= apuesta;
            resaltarMonto("PERDISTE en Dados", apuesta, false);
            resultado = "Perdio $" + formatoMonto(apuesta) + " en Dados";
        }
    }
    return resultado;
}