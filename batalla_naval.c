#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "oponente.h"

#define COLOR_TOCADO "\x1b[33m"  // Amarillo
#define COLOR_HUNDIDO "\x1b[31m" // Rojo
#define COLOR_GANADOR "\x1b[32m" // Verde
#define COLOR_RESET "\x1b[0m"    // RESET
#define TEXTO_BOLD "\x1b[1m"     // Negrita
#define COLOR_AGUA "\x1b[34m"    // Azul
#define COLOR_BARCO "\x1b[35m"    // Magenta

#define TAMANIO_TABLERO 10
#define CANTIDAD_BARCOS 5
#define LECTURA "r"
#define ESCRITURA "w"

const int COORDENADA_MINIMA = 1;

const char FORMATO_LECTURA[] = "%d;%d; %c;%d";
const int ARGUMENTO_BARCOS = 1;
const int ARGUMENTO_REPORTE = 2;
const int ARGUMENTOS_REQUERIDOS = 3;

const int ERROR_LECTURA = 1;
const int ERROR_ESCRITURA = 2;
const int ERROR_ABRIR_ARCHIVO = 3;
const int ERROR_CANTIDAD_ARGUMENTOS = 4;
const int EXITO = 0;
const int INICIALIZACION_INVALIDA = -1;
const int ERROR_GENERAL = -1;
const int INICIALIZACION_VALIDA = 0;

const char AGUA = 'A';
const char TOCADO = 'T';
const char HUNDIDO = 'H';
const char BARCO = 'B';
const char MAPA_INCOGNITO = '.';

const int CANTIDAD_BARCOS_LARGO_2 = 1;
const int CANTIDAD_BARCOS_LARGO_3 = 2;
const int CANTIDAD_BARCOS_LARGO_4 = 1;
const int CANTIDAD_BARCOS_LARGO_5 = 1;
const int LARGO_BARCO_2 = 2;
const int LARGO_BARCO_3 = 3;
const int LARGO_BARCO_4 = 4;
const int LARGO_BARCO_5 = 5;
const int LARGO_MINIMO_BARCOS = 2;
const int LARGO_MAXIMO_BARCOS = 5;

const char NORTE = 'N';
const char SUR = 'S';
const char ESTE = 'E';
const char OESTE = 'O';

typedef struct juego
{
    char tablero_propio[TAMANIO_TABLERO][TAMANIO_TABLERO];
    char tablero_enemigo[TAMANIO_TABLERO][TAMANIO_TABLERO];
    barco_t barcos_jugador[CANTIDAD_BARCOS];
    int cantidad_barcos_leidos;

    int balas_aliadas_acertadas;
    int balas_aliadas_erradas;
    int balas_enemigas_acertadas;
    int balas_enemigas_erradas;
    int barcos_enemigos_hundidos;
    int barcos_aliados_sobrevivientes;
} juego_t;

/*
PRE: -
POS: Inicializa el juego, dejando los tableros con el mapa de incógnito, los barcos sin posiciones asignadas y las variables de conteo en 0 o cantidad inicial.
*/
void inicializar_juego(juego_t *juego)
{
    for (int i = INICIALIZACION_VALIDA; i < TAMANIO_TABLERO; i++)
    {
        for (int j = INICIALIZACION_VALIDA; j < TAMANIO_TABLERO; j++)
        {
            juego->tablero_propio[i][j] = MAPA_INCOGNITO;
            juego->tablero_enemigo[i][j] = MAPA_INCOGNITO;
        }
    }
    for (int i = INICIALIZACION_VALIDA; i < CANTIDAD_BARCOS; i++)
    {
        juego->barcos_jugador[i].posiciones = NULL;
        juego->barcos_jugador[i].largo = INICIALIZACION_INVALIDA;
    }
    juego->cantidad_barcos_leidos = INICIALIZACION_VALIDA;
    juego->balas_aliadas_acertadas = INICIALIZACION_VALIDA;
    juego->balas_aliadas_erradas = INICIALIZACION_VALIDA;
    juego->balas_enemigas_acertadas = INICIALIZACION_VALIDA;
    juego->balas_enemigas_erradas = INICIALIZACION_VALIDA;
    juego->barcos_enemigos_hundidos = INICIALIZACION_VALIDA;
    juego->barcos_aliados_sobrevivientes = CANTIDAD_BARCOS;
}

/*
PRE: juego debe haber sido inicializado previamente.
POS: libera la memoria asignada a las posiciones de los barcos, si es que se asignó memoria, es decir, que no sea NULL.
    Deja los punteros a posiciones en NULL con el fin de evitar accesos inválidos
*/
void liberar_barcos_memoria(juego_t *juego)
{
    for (int i = INICIALIZACION_VALIDA; i < juego->cantidad_barcos_leidos; i++)
    {
        if (juego->barcos_jugador[i].posiciones != NULL)
        {
            free(juego->barcos_jugador[i].posiciones);
            juego->barcos_jugador[i].posiciones = NULL;
        }
    }
}

/*
PRE: juego debe haber sido inicializado previamente.
POS: Muestra el estado actual del juego, con el tablero propio y el tablero enemigo.
*/
void mostrar_juego(juego_t *juego)
{
    printf("\n"
           "    ┌───────────────────────────────┐          ┌───────────────────────────────┐\n"
           "    │     TU TABLERO (DEFENSA)      │          │   TABLERO ENEMIGO (ATAQUE)    │\n"
           "    └───────────────────────────────┘          └───────────────────────────────┘\n");
    printf("      1  2  3  4  5  6  7  8  9  10              1  2  3  4  5  6  7  8  9  10\n");
    printf("    ┌───────────────────────────────┐          ┌───────────────────────────────┐\n");

    for (int i = INICIALIZACION_VALIDA; i < TAMANIO_TABLERO; i++)
    {
        int i_aux = i + 1;
        
        // --- TABLERO PROPIO (Fila i) ---
        if (i_aux < 10) {
            printf(" %d  │ ", i_aux);
        } else {
            printf("%d  │ ", i_aux);
        }

        for (int j = INICIALIZACION_VALIDA; j < TAMANIO_TABLERO; j++)
        {
            char celda = juego->tablero_propio[i][j];
            if (celda == AGUA){
                printf(COLOR_AGUA "%c  " COLOR_RESET, celda);
            }      
            else if (celda == TOCADO){
                printf(COLOR_TOCADO "%c  " COLOR_RESET, celda);
            }
            else if (celda == HUNDIDO){
                printf(COLOR_HUNDIDO "%c  " COLOR_RESET, celda);
            }
            else{
                printf("%c  ", celda);
            }            
        }
        printf("│      "); // Cierra tablero propio y deja espacio en el medio

        // --- TABLERO ENEMIGO (Fila i) ---
        if (i_aux < 10) {
            printf(" %d  │ ", i_aux);
        } else {
            printf("%d  │ ", i_aux);
        }

        for (int j = INICIALIZACION_VALIDA; j < TAMANIO_TABLERO; j++)
        {
            char celda = juego->tablero_enemigo[i][j];
            if (celda == AGUA){
                printf(COLOR_AGUA "%c  " COLOR_RESET, celda);
            }      
            else if (celda == TOCADO){
                printf(COLOR_TOCADO "%c  " COLOR_RESET, celda);
            }
            else if (celda == HUNDIDO){
                printf(COLOR_HUNDIDO "%c  " COLOR_RESET, celda);
            }
            else{
                printf("%c  ", celda);
            }                       
        }
        printf("│\n"); 
    }
    
    printf("    └───────────────────────────────┘          └───────────────────────────────┘\n\n");
}

/*
PRE: juego debe haber sido inicializado previamente y haber leído la cantidad de barcos permitida.
POS: Valida que la composición de barcos leída corresponda a la composición reglamentaria, es decir, que haya 1 barco de largo 2, 2 barcos de largo 3, 1 barco de largo 4 y 1 barco de largo 5.
    Devuelve true si la composición es correcta, false en caso contrario.
*/
bool validar_composicion_barcos(juego_t *juego)
{
    int cont_largo_2 = INICIALIZACION_VALIDA;
    int cont_largo_3 = INICIALIZACION_VALIDA;
    int cont_largo_4 = INICIALIZACION_VALIDA;
    int cont_largo_5 = INICIALIZACION_VALIDA;
    for (int i = INICIALIZACION_VALIDA; i < juego->cantidad_barcos_leidos; i++)
    {
        if (juego->barcos_jugador[i].largo == LARGO_BARCO_2)
        {
            cont_largo_2++;
        }
        else if (juego->barcos_jugador[i].largo == LARGO_BARCO_3)
        {
            cont_largo_3++;
        }
        else if (juego->barcos_jugador[i].largo == LARGO_BARCO_4)
        {
            cont_largo_4++;
        }
        else if (juego->barcos_jugador[i].largo == LARGO_BARCO_5)
        {
            cont_largo_5++;
        }
    }
    return (cont_largo_2 == CANTIDAD_BARCOS_LARGO_2 && cont_largo_3 == CANTIDAD_BARCOS_LARGO_3 && cont_largo_4 == CANTIDAD_BARCOS_LARGO_4 && cont_largo_5 == CANTIDAD_BARCOS_LARGO_5);
}
/*
PRE: debe entrar un largo de barco a validar.
POS: Valida que el largo del barco esté entre el largo mínimo, 2, y máximo, 5, permitido. Devuelve true si el largo es válido, false en caso contrario.
*/
int validar_largo_barco(int largo)
{
    return (largo >= LARGO_MINIMO_BARCOS && largo <= LARGO_MAXIMO_BARCOS);
}

/*
PRE: debe entrar una cantidad de barcos a validar.
POS: Valida que la cantidad de barcos sea la correcta. Devuelve true si la cantidad es válida, false en caso contrario. Es válida la cantidad si es igual a 5.
*/
int validar_cantidad_barcos_cargados(int cantidad)
{
    if (cantidad != CANTIDAD_BARCOS)
    {
        return ERROR_LECTURA;
    }
    return EXITO;
}
/*
PRE: debe entrar una dirección a validar.
POS: Valida que la dirección sea una de las permitidas, es decir, N, S, E u O. Devuelve true si la dirección es válida, false en caso contrario.
*/
int validar_direccion(char direccion)
{
    if ((direccion != NORTE) && (direccion != SUR) && (direccion != ESTE) && (direccion != OESTE))
    {
        return ERROR_LECTURA;
    }
    return EXITO;
}
/*
PRE: debe entrar una fila, columna, dirección, largo y cantidad de barcos a validar.
POS: Valida que los datos del barco sean correctos. Devuelve true si los datos son válidos, false en caso contrario. Se validan la cantidad de barcos, que la posición (FILA, COLUMNA) esté dentro del mapa, que la dirección sea válida y que el largo del barco sea válido.
*/
int validar_datos_barco(int fila, int columna, char direccion, int largo, int cant_barcos)
{
    if (cant_barcos >= CANTIDAD_BARCOS)
    {
        printf("¡Error de lectura! Se han leído más barcos de los permitidos.\n");
        return ERROR_LECTURA;
    }
    if ((fila < COORDENADA_MINIMA) || (fila > TAMANIO_TABLERO) || (columna < COORDENADA_MINIMA) || (columna > TAMANIO_TABLERO))
    {
        printf("¡Error de lectura! Posición inicial fuera del mapa: %d;%d\n", fila, columna);
        return ERROR_LECTURA;
    }
    if (validar_direccion(direccion) != EXITO)
    {
        printf("¡Error de lectura! Dirección inválida: %c. \n Solo puede ser: -S-, -N-, -E-, -O-\n", direccion);
        return ERROR_LECTURA;
    }
    if (!validar_largo_barco(largo))
    {
        printf("¡Error de lectura! Largo de barco inválido: %d\n", largo);
        return ERROR_LECTURA;
    }

    return EXITO;
}

/*
PRE: debe entrar una dirección y dos punteros a enteros de fila y columna.
POS: Carga en los punteros de fila y columna la el tipo de dirección del barco correspondiente a la dirección ingresada.
*/
void cargar_direccion(char direccion, int *posicion_fila, int *posicion_columna)
{
    if (direccion == NORTE)
    {
        *posicion_fila = -1;
    }
    else if (direccion == SUR)
    {
        *posicion_fila = 1;
    }
    else if (direccion == ESTE)
    {
        *posicion_columna = 1;
    }
    else if (direccion == OESTE)
    {
        *posicion_columna = -1;
    }
}
/*
PRE: debe entrar la posición inicial del barco en el tablero, la dirección del barco, el largo del barco, un arreglo de coordenadas para cargar las posiciones del barco y una matriz de ocupación para validar que las posiciones no estén ocupadas.
POS: Valida que las posiciones que ocuparía el barco, a partir de la posición inicial y la dirección, estén dentro del tablero y no estén ocupadas por otro barco.
    Si las posiciones son válidas, las carga en el arreglo de coordenadas. Devuelve true si las posiciones son válidas y se cargaron correctamente, false en caso contrario.
*/
bool validar_y_cargar_posiciones_barco(int fila_matriz, int columna_matriz, int posicion_fila, int posicion_columna, int largo_barco, coordenada_t *posiciones, bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    int fila_actual = INICIALIZACION_INVALIDA;
    int columna_actual = INICIALIZACION_INVALIDA;
    bool posicion_valida = true;

    for (int i = INICIALIZACION_VALIDA; i < largo_barco && posicion_valida; i++)
    {
        fila_actual = fila_matriz + (posicion_fila * i);
        columna_actual = columna_matriz + (posicion_columna * i);

        if ((fila_actual < INICIALIZACION_VALIDA) || (fila_actual > TAMANIO_TABLERO) || (columna_actual < INICIALIZACION_VALIDA) || (columna_actual > TAMANIO_TABLERO) || (matriz_ocupacion[fila_actual][columna_actual]))
        {
            posicion_valida = false;
        }
        else
        {
            posiciones[i].fila = fila_actual;
            posiciones[i].columna = columna_actual;
        }
    }
    return posicion_valida;
}
/*
PRE: debe entrar el juego, el largo del barco y un arreglo de coordenadas con las posiciones del barco.
POS: Carga el barco en el juego, asignándole el largo y las posiciones correspondientes.
*/
void asignar_barco(juego_t *juego, int largo_barco, coordenada_t *posiciones)
{
    barco_t *nuevo_barco = &(juego->barcos_jugador[juego->cantidad_barcos_leidos]);
    nuevo_barco->largo = largo_barco;
    nuevo_barco->posiciones = posiciones;
}
/*
PRE: debe entrar el juego, un arreglo de coordenadas con las posiciones del barco, el largo del barco y una matriz de ocupación para marcar las posiciones ocupadas por el barco.
POS: Marca en el tablero propio del juego las posiciones del barco con el símbolo de barco y en la matriz de ocupación las posiciones ocupadas por el barco actual ingresado con true.
*/
void dibujar_posiciones_barco(juego_t *juego, coordenada_t *posiciones, int largo_barco, bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    for (int i = INICIALIZACION_VALIDA; i < largo_barco; i++)
    {
        matriz_ocupacion[posiciones[i].fila][posiciones[i].columna] = true;
        juego->tablero_propio[posiciones[i].fila][posiciones[i].columna] = BARCO;
    }
}
/*
PRE: debe entrar una matriz de ocupación del tablero.
POS: Inicializa la matriz de ocupación del tablero con false en todas las posiciones, indicando que AÙN no hay posiciones ocupadas por barcos.
*/
void inicializar_tablero_ocupacion(bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    for (int i = INICIALIZACION_VALIDA; i < TAMANIO_TABLERO; i++)
    {
        for (int j = INICIALIZACION_VALIDA; j < TAMANIO_TABLERO; j++)
        {
            matriz_ocupacion[i][j] = false;
        }
    }
}
/*
PRE: debe entrar el archivo de barcos del jugador YA ABIERTOy el juego inicializado.
POS: Lee los datos de los barcos del jugador desde el archivo, valida que sean correctos, carga los barcos en el juego
    y marca las posiciones de los barcos en el tablero propio.
    Devuelve EXITO si se cargaron correctamente todos los barcos, ERROR_LECTURA si hubo un error en la lectura o validación de los datos de los barcos
    o si la composición final de la flota no es la reglamentaria o faltan barcos.
*/
int cargar_barcos_propios(FILE *barcos_jugador, juego_t *juego)
{
    int fila_inicializacion = INICIALIZACION_INVALIDA;
    int columna_inicializacion = INICIALIZACION_INVALIDA;
    int largo_barco = INICIALIZACION_INVALIDA;
    char direccion;
    bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO];
    inicializar_tablero_ocupacion(matriz_ocupacion);

    bool hubo_error = false;
    int codigo_retorno = EXITO;

    while (!hubo_error && (fscanf(barcos_jugador, FORMATO_LECTURA, &fila_inicializacion, &columna_inicializacion, &direccion, &largo_barco)) != EOF)
    {
        if (validar_datos_barco(fila_inicializacion, columna_inicializacion, direccion, largo_barco, juego->cantidad_barcos_leidos) != EXITO)
        {
            hubo_error = true;
            codigo_retorno = ERROR_LECTURA;
        }
        else
        {
            int posicion_fila = INICIALIZACION_VALIDA;
            int posicion_columna = INICIALIZACION_VALIDA;
            cargar_direccion(direccion, &posicion_fila, &posicion_columna);
            coordenada_t *posiciones = malloc(sizeof(coordenada_t) * (size_t)largo_barco);
            if (!posiciones)
            {
                hubo_error = true;
                codigo_retorno = ERROR_LECTURA;
            }
            else
            {
                int fila_matriz = fila_inicializacion - 1;
                int columna_matriz = columna_inicializacion - 1;

                if (!validar_y_cargar_posiciones_barco(fila_matriz, columna_matriz, posicion_fila, posicion_columna, largo_barco, posiciones, matriz_ocupacion))
                {
                    free(posiciones);
                    printf("¡Error de lectura! El barco ocupa posiciones inválidas o superpuestas.\n");
                    hubo_error = true;
                    codigo_retorno = ERROR_LECTURA;
                }
                else
                {
                    asignar_barco(juego, largo_barco, posiciones);
                    dibujar_posiciones_barco(juego, posiciones, largo_barco, matriz_ocupacion);
                    juego->cantidad_barcos_leidos++;
                }
            }
        }
    }
    fclose(barcos_jugador);

    if (!hubo_error)
    {
        if (validar_cantidad_barcos_cargados(juego->cantidad_barcos_leidos) != EXITO || !validar_composicion_barcos(juego))
        {
            printf("¡Error! La composición final de la flota no es la reglamentaria o faltan barcos.\n");
            codigo_retorno = ERROR_LECTURA;
        }
    }

    return codigo_retorno;
}
/*
PRE: debe entrar un barco y una coordenada de disparo validas
POS: Valida si el disparo ingresado por el enemigo coincide con alguna de las posiciones del barco. Devuelve true si el barco fue tocado, false en caso contrario.
*/
bool barco_fue_tocado(barco_t barco_actual, coordenada_t disparo)
{
    bool tocado = false;
    int i = INICIALIZACION_VALIDA;
    while (i < barco_actual.largo && !tocado)
    {
        if ((barco_actual.posiciones[i].fila == disparo.fila) && (barco_actual.posiciones[i].columna == disparo.columna))
        {
            tocado = true;
        }
        i++;
    }
    return tocado;
}
/*
PRE: debe entrar el tablero propio del juego y un barco actual a analizar con sus posiciones cargadas.
POS: Valida si el barco actual fue hundido, es decir, si todas sus posiciones fueron tocadas. Devuelve true si el barco fue hundido, false en caso contrario.
*/
bool enemigo_hunde_barco(char tablero_propio[TAMANIO_TABLERO][TAMANIO_TABLERO], barco_t *barco)
{
    bool hundido = true;
    for (int i = INICIALIZACION_VALIDA; i < barco->largo; i++)
    {
        if (tablero_propio[barco->posiciones[i].fila][barco->posiciones[i].columna] != TOCADO)
        {
            hundido = false;
        }
    }
    return hundido;
}
/*
PRE: debe entrar el tablero propio del juego y un barco hundido con sus posiciones cargadas.
POS: Modifica el tablero propio del juego, marcando con el símbolo de hundido las posiciones del barco que fue hundido por el enemigo. ('H')
*/
void modificar_tablero_propio_hundido(char tablero_propio[TAMANIO_TABLERO][TAMANIO_TABLERO], barco_t *barco)
{
    for (int i = INICIALIZACION_VALIDA; i < barco->largo; i++)
    {
        tablero_propio[barco->posiciones[i].fila][barco->posiciones[i].columna] = HUNDIDO;
    }
}
/*
PRE: debe entrar un puntero a la cantidad de barcos aliados sobrevivientes y un puntero a la cantidad de balas enemigas acertadas.
POS: Modifica la cantidad de barcos aliados sobrevivientes, restando 1, y muestra un mensaje informando al jugador que el enemigo ha hundido uno de sus barcos
    y cuántos barcos aliados sobrevivientes le quedan.
*/
void consecuencia_barco_hundido(int *barcos_aliados_sobrevivientes, int *balas_enemigas_acertadas)
{
    printf(TEXTO_BOLD COLOR_HUNDIDO "¡PELIGRO! El enemigo ha HUNDIDO uno de tus barcos.\n" COLOR_RESET);
    printf(" Te quedan " TEXTO_BOLD "%d" COLOR_RESET " barcos aliados en pie.\n", *barcos_aliados_sobrevivientes - 1);
    (*barcos_aliados_sobrevivientes)--;
}
/*
PRE: -
POS: Muestra un mensaje informando al jugador que el enemigo ha tocado uno de sus barcos.
*/
void comunicacion_barco_tocado()
{
    printf(TEXTO_BOLD COLOR_TOCADO "¡ALERTA! El enemigo impactó en uno de tus barcos.\n" COLOR_RESET);
}
/*
PRE: debe entrar el juego inicializado y la coordenada de disparo del enemigo que fue acertada en un barco del jugador.
POS: Modifica el tablero propio del juego, marcando con el símbolo de tocado la posición del disparo acertado por el enemigo, y modifica la cantidad de balas enemigas acertadas, sumando 1.
*/
void consecuencia_enemigo_acierta(juego_t *juego, coordenada_t disparo)
{
    juego->tablero_propio[disparo.fila][disparo.columna] = TOCADO;
    juego->balas_enemigas_acertadas++;
}
/*
PRE: debe entrar el juego inicializado, especialmente con los barcos del jugador ya cargados, y una coordenada de disparo del enemigo.
POS: Analiza el disparo realizado por el enemigo, validando si acertó o erró el disparo. Si el disparo fue acertado, se analiza si el barco tocado fue hundido o no,
    y se actualiza el tablero propio del juego y las variables de conteo correspondientes, mostrando mensajes informativos al jugador.
    Si el disparo fue erróneo, se actualiza el tablero propio del juego marcando el agua y se incrementa la cantidad de balas enemigas erradas, mostrando un mensaje informando al jugador que el enemigo disparó al agua.
*/
void analizar_disparo_enemigo(juego_t *juego, coordenada_t disparo)
{
    if (juego->tablero_propio[disparo.fila][disparo.columna] == BARCO)
    {
        consecuencia_enemigo_acierta(juego, disparo);

        for (int i = INICIALIZACION_VALIDA; i < CANTIDAD_BARCOS; i++)
        {
            barco_t *barco_actual = &(juego->barcos_jugador[i]);
            if (barco_fue_tocado(*barco_actual, disparo))
            {
                if (enemigo_hunde_barco(juego->tablero_propio, barco_actual))
                {
                    modificar_tablero_propio_hundido(juego->tablero_propio, barco_actual);
                    consecuencia_barco_hundido(&(juego->barcos_aliados_sobrevivientes), &(juego->balas_enemigas_acertadas));
                }
                else
                {
                    comunicacion_barco_tocado();
                }
            }
        }
    }
    else if (juego->tablero_propio[disparo.fila][disparo.columna] == MAPA_INCOGNITO)
    {
        juego->tablero_propio[disparo.fila][disparo.columna] = AGUA;
        juego->balas_enemigas_erradas++;
        printf(TEXTO_BOLD COLOR_AGUA " ¡El enemigo disparó al AGUA!\n" COLOR_RESET);
    }
}
/*
PRE: debe entrar el archivo de reporte abierto para escritura y el juego con las variables de conteo actualizadas.
POS: Escribe en el archivo de reporte la cantidad de balas aliadas acertadas, balas aliadas erradas, balas enemigas acertadas, balas enemigas erradas, barcos enemigos hundidos y barcos aliados sobrevivientes.
*/
void escribir_reporte(FILE *reporte_archivo, juego_t *juego)
{
    fprintf(reporte_archivo, "Balas aliadas acertadas: %d\n", juego->balas_aliadas_acertadas);
    fprintf(reporte_archivo, "Balas aliadas erradas: %d\n", juego->balas_aliadas_erradas);
    fprintf(reporte_archivo, "Balas enemigas acertadas: %d\n", juego->balas_enemigas_acertadas);
    fprintf(reporte_archivo, "Balas enemigas erradas: %d\n", juego->balas_enemigas_erradas);
    fprintf(reporte_archivo, "Barcos enemigos hundidos: %d\n", juego->barcos_enemigos_hundidos);
    fprintf(reporte_archivo, "Barcos aliados sobrevivientes: %d\n", juego->barcos_aliados_sobrevivientes);
    printf("Reporte escrito correctamente.\n");
}
/*
PRE: debe entrar la cantidad de argumentos ingresados al ejecutar el programa.
POS: Valida que la cantidad de argumentos sea la correcta, es decir, 3, incluyendo el nombre del programa, el archivo de barcos y el archivo de reporte.
    Devuelve EXITO si la cantidad de argumentos es correcta, ERROR_CANTIDAD_ARGUMENTOS en caso contrario, mostrando un mensaje informativo al usuario sobre la forma correcta de ejecutar el programa.
*/
int verificar_cant_argumentos(int argc)
{
    if (argc != ARGUMENTOS_REQUERIDOS)
    {
        return ERROR_CANTIDAD_ARGUMENTOS;
    }
    return EXITO;
}
/*
PRE: debe entrar una fila y columna de disparo ingresados por el usuario y el tablero enemigo del juego.
POS: Valida que el disparo ingresado por el usuario sea válido, es decir, que la fila y columna estén dentro del rango permitido, que no se haya disparado previamente a esa coordenada y que el formato de entrada sea correcto. Devuelve true si el disparo es válido, false en caso contrario.
*/
bool disparo_valido(int fila, int columna, char tablero_enemigo[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    if ((fila < COORDENADA_MINIMA) || (fila > TAMANIO_TABLERO) || (columna < COORDENADA_MINIMA) || (columna > TAMANIO_TABLERO) || tablero_enemigo[fila - 1][columna - 1] != MAPA_INCOGNITO)
    {
        return false;
    }
    return true;
}
/*
PRE: debe entrar el tablero enemigo del juego, punteros a las variables de conteo de balas acertadas, balas erradas y barcos hundidos del jugador, el resultado del disparo del jugador y la coordenada del disparo del jugador, TODO inicializado como corresponde.
POS: Modifica el tablero enemigo del juego, marcando con el símbolo correspondiente el resultado del disparo del jugador, y modifica las variables de conteo de balas acertadas, balas erradas y barcos hundidos del jugador según corresponda al resultado del disparo.
    Además, muestra un mensaje informando al jugador el resultado de su disparo.
*/
void consecuencia_resultado_disparo_jugador(char tablero_enemigo[TAMANIO_TABLERO][TAMANIO_TABLERO], int *balas_acertadas, int *balas_erradas, int *barcos_hundidos, char resultado, coordenada_t disparo)
{
    printf(TEXTO_BOLD " ├─ TELEMETRÍA DE " COLOR_GANADOR "TU ATAQUE " COLOR_RESET TEXTO_BOLD "──> Coordenada: [" COLOR_GANADOR "%d;%d" COLOR_RESET TEXTO_BOLD "]\n" COLOR_RESET, disparo.fila + 1, disparo.columna + 1);
    printf(" │  Resultado: ");
    if (resultado == AGUA)
    {
        printf(COLOR_AGUA "¡Agua! No había nada ahí.\n" COLOR_RESET);
        (*balas_erradas)++;
        tablero_enemigo[disparo.fila][disparo.columna] = AGUA;
    }
    else if (resultado == TOCADO)
    {
        printf(COLOR_TOCADO " ¡IMPACTO! Has tocado un barco enemigo.\n" COLOR_RESET);
        (*balas_acertadas)++;
        tablero_enemigo[disparo.fila][disparo.columna] = TOCADO;
    }
    else if (resultado == HUNDIDO)
    {
        printf(COLOR_HUNDIDO "¡HUNDISTE UN BARCO ENEMIGO!\n" COLOR_RESET);
        (*balas_acertadas)++;
        (*barcos_hundidos)++;
        tablero_enemigo[disparo.fila][disparo.columna] = HUNDIDO;
    }
}
/*
PRE: debe entrar punteros a enteros de fila y columna para cargar el disparo del usuario.
POS: Solicita al usuario que ingrese un disparo en el formato FILA;COLUMNA, y carga la fila y columna ingresadas en los punteros correspondientes. Solo realiza la lectura de la entrada del usuario.
*/
void obtener_disparo_usuario(int *fila, int *columna)
{
    printf(TEXTO_BOLD "Simbolos: " COLOR_HUNDIDO "H" COLOR_RESET  " para hundido, " COLOR_TOCADO "T" COLOR_RESET  " para tocado, " COLOR_AGUA "A" COLOR_RESET " para agua, " COLOR_BARCO "B" COLOR_RESET " para barco\n");
    printf("Ingrese su disparo (FILA;COLUMNA): ");
    scanf(" %d;%d", fila, columna);
}
/*
PRE: debe entrar un puntero a una coordenada para cargar el disparo del usuario y el tablero enemigo del juego.
POS: Solicita al usuario que ingrese un disparo, valida que el disparo sea válido utilizando la función disparo_valido, y carga la fila y columna del disparo en el puntero a coordenada ingresado. Si el disparo no es válido, muestra un mensaje informando al usuario que el disparo es inválido y solicita nuevamente el ingreso del disparo hasta que se ingrese un disparo válido. Además, ajusta la fila y columna del disparo para que correspondan a los índices de las matrices del juego, restando 1 a cada uno, ya que el usuario ingresa las coordenadas en formato 1-10 pero las matrices del juego están indexadas desde 0.
*/
void obtener_disparo_valido(coordenada_t *disparo, char tablero_enemigo[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    do
    {
        obtener_disparo_usuario(&disparo->fila, &disparo->columna);
        if (!disparo_valido(disparo->fila, disparo->columna, tablero_enemigo))
        {
            printf(COLOR_HUNDIDO "❌ Coordenada inválida o ya atacada. Rango válido [1-%d]. Reintente.\n" COLOR_RESET, TAMANIO_TABLERO);
        }
    } while (!disparo_valido(disparo->fila, disparo->columna, tablero_enemigo));
    disparo->fila = disparo->fila - 1;
    disparo->columna = disparo->columna - 1;
}
/*
PRE: debe entrar la coordenada del disparo del jugador, el oponente con su función de recibir disparo implementada y el juego con los tableros y variables de conteo inicializados.
POS: Realiza el disparo del jugador utilizando la función de recibir disparo del oponente
*/
void consecuencia_disparo_jugador(coordenada_t disparo, oponente_t *oponente, juego_t *juego)
{
    char resultado_jugador = oponente_recibe_disparo(oponente, disparo);
    consecuencia_resultado_disparo_jugador(juego->tablero_enemigo, &(juego->balas_aliadas_acertadas), &(juego->balas_aliadas_erradas), &(juego->barcos_enemigos_hundidos), resultado_jugador, disparo);
}
/*
PRE: debe entrar el oponente con su función de realizar disparo implementada y el juego con los tableros y variables de conteo inicializados.
POS: Realiza el turno del oponente, obteniendo la coordenada del disparo del oponente utilizando su función de realizar disparo, mostrando un mensaje informando al jugador las coordenadas del disparo del oponente, y analizando el disparo del oponente utilizando la función analizar_disparo_enemigo para actualizar el tablero propio del juego y las variables de conteo correspondientes.
*/
void turno_oponente(oponente_t *oponente, juego_t *juego)
{
    coordenada_t disp_enemigo = oponente_realiza_disparo(oponente);
    printf(TEXTO_BOLD "El radar detecta ataque enemigo en [%d;%d]... " COLOR_RESET, disp_enemigo.fila + 1, disp_enemigo.columna + 1);
    analizar_disparo_enemigo(juego, disp_enemigo);
}
/*
PRE: debe entrar el oponente con sus funciones de recibir disparo y realizar disparo implementadas, y el juego con los tableros y variables de conteo inicializados.
POS: Realiza los turnos de la batalla naval, alternando entre el jugador y el oponente, hasta que se cumpla la condición de fin de juego, es decir,
    que el jugador haya hundido todos los barcos enemigos o que el enemigo haya hundido toda la flota del jugador.  En cada turno, se obtiene un disparo
    válido del jugador, se realiza el disparo del jugador utilizando la función de recibir disparo del oponente, se analiza el resultado del disparo del
    jugador y si el juego no ha terminado, se realiza el turno del oponente utilizando su función de realizar disparo y analizando el disparo del oponente.
    Después de cada turno, se muestra el estado actual del juego utilizando la función mostrar_juego.
*/
void empezar_turnos_batalla(oponente_t *oponente, juego_t *juego)
{
    coordenada_t disparo_jugador;
    printf("Empieza la batalla naval...!\n");
    while (juego->barcos_aliados_sobrevivientes > INICIALIZACION_VALIDA && juego->barcos_enemigos_hundidos < CANTIDAD_BARCOS)
    {
        printf("\n" TEXTO_BOLD COLOR_GANADOR);
        printf("╔════════════════════════════════════════╗\n");
        printf("║             TURNO JUGADOR              ║\n");
        printf("╚════════════════════════════════════════╝\n");
        printf(COLOR_RESET);
        obtener_disparo_valido(&disparo_jugador, juego->tablero_enemigo);
        system("clear");
        consecuencia_disparo_jugador(disparo_jugador, oponente, juego);
        if (juego->barcos_enemigos_hundidos < CANTIDAD_BARCOS)
        {
            printf("\n" TEXTO_BOLD COLOR_HUNDIDO);
            printf("╔════════════════════════════════════════╗\n");
            printf("║            TURNO ENEMIGO               ║\n");
            printf("╚════════════════════════════════════════╝\n");
            printf(COLOR_RESET);
            turno_oponente(oponente, juego);
        }
        mostrar_juego(juego);
    }
}
/*
PRE: debe entrar el juego con los tableros y variables de conteo actualizados al finalizar la batalla naval.
POS: Muestra un mensaje final al jugador informando si ganó o perdió la batalla naval, dependiendo de si hundió todos los barcos enemigos o si el enemigo hundió toda su flota, respectivamente.
*/
void mostrar_mensaje_final(juego_t juego)
{
    printf("\n============================================================\n");
    if (juego.barcos_enemigos_hundidos == CANTIDAD_BARCOS)
    {
        printf(TEXTO_BOLD COLOR_GANADOR "¡VICTORIA! Es el fin de la batalla... hundiste la flota enemiga.\n" COLOR_RESET);
    }
    else
    {
        printf(TEXTO_BOLD COLOR_HUNDIDO "DERROTA... Tu flota descansa en el fondo del océano....\n" COLOR_RESET);
        printf("   La máquina ha ganado esta batalla.\n");
    }
    printf("==============================================================\n\n");
}
/*
PRE: debe entrar el estado de carga del juego, que puede ser ERROR_ABRIR_ARCHIVO o ERROR_LECTURA.
POS: Muestra un mensaje de error al jugador dependiendo del estado de carga ingresado.
    Si el estado de carga es ERROR_ABRIR_ARCHIVO, muestra un mensaje informando que hubo un error al abrir el archivo.
    Si el estado de carga es ERROR_LECTURA, muestra un mensaje informando que hubo un error en la lectura de los datos del archivo.
*/
void mostrar_mensaje_error(int estado_carga)
{
    if (estado_carga == ERROR_ABRIR_ARCHIVO)
    {
        printf("Error de abrir el archivo.\n");
    }
    else if (estado_carga == ERROR_LECTURA)
    {
        printf("Error de lectura de los datos del archivo.\n");
    }
    else if (estado_carga == ERROR_ESCRITURA)
    {
        printf("Error de escritura del archivo de reporte.\n");
    }
    else if (estado_carga == ERROR_CANTIDAD_ARGUMENTOS)
    {
        printf("Error de cantidad de argumentos. Se DEBE ingresar: <programa_ejecutable> <archivo_barcos> <archivo_reporte>\n");
    }
}

int main(int argc, char *argv[])
{
    int estado_carga = INICIALIZACION_INVALIDA;
    estado_carga = verificar_cant_argumentos(argc);
    if (estado_carga != EXITO)
    {
        mostrar_mensaje_error(estado_carga);
        return estado_carga;
    }

    juego_t juego;
    inicializar_juego(&juego);
    FILE *barcos_jugador = fopen(argv[ARGUMENTO_BARCOS], LECTURA);
    if (!barcos_jugador)
    {
        mostrar_mensaje_error(ERROR_ABRIR_ARCHIVO);
        return ERROR_ABRIR_ARCHIVO;
    }
    estado_carga = cargar_barcos_propios(barcos_jugador, &juego);

    if (estado_carga != EXITO)
    {
        mostrar_mensaje_error(estado_carga);
        liberar_barcos_memoria(&juego);
        return estado_carga;
    }

    oponente_t *oponente = oponente_crear(juego.barcos_jugador);
    if (!oponente)
    {
        liberar_barcos_memoria(&juego);
        return ERROR_LECTURA;
    }
    mostrar_juego(&juego);
    empezar_turnos_batalla(oponente, &juego);
    mostrar_mensaje_final(juego);
    oponente_destruir(oponente);
    liberar_barcos_memoria(&juego);
    FILE *reporte_archivo = fopen(argv[ARGUMENTO_REPORTE], ESCRITURA);
    if (!reporte_archivo)
    {
        mostrar_mensaje_error(ERROR_LECTURA);
        return ERROR_ESCRITURA;
    }

    escribir_reporte(reporte_archivo, &juego);
    fclose(reporte_archivo);

    return EXITO;
}