#include <iostream>
#include <cstdlib>
#include <ctime>

//incluye prints con proposito de tests en consola


using namespace std;

struct playerInfo {
    //consstruir objetos tipos playerInfo que contienen el nickname y el color de disco

    char nickname[81];
    char disc;
};

//prototipos: para tener a mano los nombres y parametros de las funciones; nos ayuda a controlar el orden en el que aparecen


void gameMode(int choice);
void singlePlayer();
void multiplayer();
char** createBoard(int rows, int columns);
void printBoard(char** &board, int rows, int columns);
int playerAction(char** &board, int rows, int columns, playerInfo player);
void placingDiscMP(char** &board, int rows, int columns, playerInfo player, int slot);
bool checkWin(char** &board, int rows, int columns, playerInfo player);
int phanMove(char** &board, int rows, int columns, playerInfo ai, playerInfo opponent);
bool crecerTablero();
void growBoard(char** &board, int &rows, int &cols);
int findAvailableRow(char**&board, int rows, int columns, int chosenColumn);
int evaluateVertical (char**& board, playerInfo player, int availableRow, int chosenColumn, int rows, int columns);
int evaluateHorizontal (char**& board, playerInfo player, int availableRow, int chosenColumn, int rows, int columns);
int evaluateDiagonal(char**& board, playerInfo player, int availableRow, int chosenColumn, int rows, int columns);
int evaluatingBoard (char ** &board, playerInfo player, int rows, int cols, int chosenColumn);


int main() {
        //solo para pruebas en la consola:
    int modeSelection;

    printf("Welcome to Connect4! To begin, please press '1' to select single player mode, or '2' to select multiplayer mode: ");
    scanf("%d", &modeSelection);

    while (modeSelection != 1 && modeSelection != 2) {
        printf("That is not a valid Game Mode. Please select either '1' for single player or '2' for multiplayer: ");
        scanf("%d", &modeSelection);
    }

    if (modeSelection == 1) {
        printf("Great, you've chosen single player mode!\n");
        singlePlayer();
    } else if (modeSelection == 2) {
        printf("Great, you've chosen multiplayer mode!\n");
        multiplayer();
    }
    return 0;
}

void singlePlayer() {
    //este metodo se encargara de manejar el loop del singleplayer. llamara a todas las funciones necesarias y las mantendra con un do while (condicion: que no haya un ganador)
    //todos los prints en esta seccion son para pruebas en la consola; no sirven en la interfaz grafica
    playerInfo player;
    playerInfo ia;
    printf("Please enter your nickname: ");
    scanf("%s", player.nickname);
    player.disc = 'r';
    printf("Please enter AI's nickname: ");
    scanf("%s", ia.nickname);
    ia.disc = 'b';

    printf("Welcome, %s! Today you, the red disc player, will be playing against an AI, the blue disc player. As soon as you're ready, start moving around your disc with the arrow controller on your screen until you've selected your desired position, and press ENTER to drop it. The AI will follow automatically. Good luck!\n", player.nickname);

    int rows = 6;
    int columns = 7;
    char** board = createBoard(rows, columns);

    bool contGame = true; //contGame depende de la existencia de un ganador

    do {
        bool decision = crecerTablero();
        if (decision) {
            growBoard(board, rows, columns);
        }
        int selectedMove = playerAction(board, rows, columns, player);
        placingDiscMP(board, rows, columns, player, selectedMove);
        printBoard(board, rows, columns);
        contGame = !checkWin(board, rows, columns, player); //si no hay gane, tiene que continuar el juego (por eso lo invierte)
        if (!contGame) {
            printf("Congratulations %s, you've won!", player.nickname);
        } 

        if (contGame) {
            printf("It's %s's turn: \n", ia.nickname);
            int iaMove = phanMove(board, rows, columns, ia, player);
            printf("IA move is  %d \n",iaMove);
            placingDiscMP(board, rows, columns, ia, iaMove);
            //printBoard(board, rows, columns);
            contGame = checkWin(board, rows, columns, ia);
            printBoard(board, rows, columns);
            
        }
    } while (!checkWin(board, rows, columns, player) && !checkWin(board, rows, columns, ia));
}

void multiplayer() {
    //este metodo se encargara de manejar el loop del multiplayer. llamara a todas las funciones necesarias y las mantendra con un do while (condicion: que no haya un ganador)
    //todos los prints en esta seccion son para pruebas en la consola; no sirven en la interfaz grafica
    playerInfo playerOne;
    playerInfo playerTwo;

    printf("First player, please enter your nickname: ");
    scanf("%s", playerOne.nickname);
    playerOne.disc = 'r';
    printf("Welcome, %s! Today you'll be playing as red.\n", playerOne.nickname);

    printf("Second player, please enter your nickname: ");
    scanf("%s", playerTwo.nickname);
    playerTwo.disc = 'b';
    printf("Welcome, %s! Today you'll be playing as blue.\n", playerTwo.nickname);

    printf("When %s is ready to play, begin moving the disc around the board with the arrow controllers, and click ENTER to drop it. As soon as the disc has entered its respective slot, %s may proceed moving around the disc. Good luck!\n", playerOne.nickname, playerTwo.nickname);
    
    int rows = 6;
    int columns = 7;
    char** board = createBoard(rows, columns);

    bool contGame = true;

    do {
        bool decision = crecerTablero();
        if (decision) {
            growBoard(board, rows, columns);
        }
        int selectedMoveOne = playerAction(board, rows, columns, playerOne);
        placingDiscMP(board, rows, columns, playerOne, selectedMoveOne);
        printBoard(board, rows, columns);
        contGame = !checkWin(board, rows, columns, playerOne); //si no hay gane, tiene que continuar el juego (por eso lo invierte)
        if (!contGame) {
            printf("Congratulations %s, you've won!", playerOne.nickname);
        } 

        if (contGame) {
            bool decision = crecerTablero();
        if (decision) {
            growBoard(board, rows, columns);
        }
            int selectedMoveTwo = playerAction(board, rows, columns, playerTwo);
            placingDiscMP(board, rows, columns, playerTwo, selectedMoveTwo);
            printBoard(board, rows, columns);
            if (checkWin(board, rows, columns, playerTwo)) {
                printf("Congratulations %s, you've won!", playerTwo.nickname);
            }
        }
    } while (!checkWin(board, rows, columns, playerOne) && !checkWin(board, rows, columns, playerTwo));
}

char** createBoard(int rows, int columns) {
    //este metodo crea la matriz de un solo con malloc (tambien sera llamado cuando se agrande). Se inicia con dimensiones de 6 y 7. cuando la crea, la rellena de espacios vacios representados por '_'
    char** board = (char**)malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; ++i) {
        board[i] = (char*)malloc(columns * sizeof(char));
        for (int j = 0; j < columns; ++j) {
            board[i][j] = '_';
        }
    }
    return board;
} 

void printBoard(char** &board, int rows, int columns) {
    //esta funcion imprime exactamente como se ve la matriz en consola
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}

bool crecerTablero() {
    //con la variable de tipo bool que se obtiene de esta funcion es que se decide si se llama la funcion para crecer el tablero
    char decision;
    printf("Wish to grow board?");
    cin >> decision;
    if (decision == 'n') {
        return false;
    } else if (decision == 'y') {
        return true;
    }

    return false;
}

int playerAction(char** &board, int rows, int columns, playerInfo player) {
    //esta funcion se encarga de manejar si el input obtenido del usuario es valido
    //todo lo impreso en esta seccion es para pruebas en la consola
    int slot;

    do {
        printf("It's your turn, %s. Please drop your disc in the desired slot: ", player.nickname);
        scanf("%d", &slot);
        if (slot < 1 || slot > columns) {
            printf("That is not a valid slot. Please select a valid slot.\n");
            //vuelve a pedir input si el slot seleccionado se sale de las dimensiones de la matriz
        } else if (board[0][slot - 1] != '_') {
            printf("That slot is already full. Please select a different slot.\n");
            slot = -1; //si esta llena, resetea el slot a -1 para que no se salga del loop hasta que no haya input valido
        }
    } while (slot < 1 || slot > columns); 

    return slot - 1;
}

bool checkWin(char** &board, int rows, int columns, playerInfo player) {
    /*esta funcion busca 4 consecutivos del mismo tipo ya sea en horizontal, vertical o diagonal en ambos sentidos, comparandolos entre si. si encuentra 4 juntos, retorna true para que en la 
    funcion de multi o single player, se salga del loop contGame*/

    for (int i = 0; i <= rows - 4; ++i) {
        for (int j = 0; j < columns; ++j) {
            if (board[i][j] == player.disc && board[i+1][j] == player.disc && board[i+2][j] == player.disc && board[i+3][j] == player.disc) {
                return true;
            }
        }
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j <= columns - 4; ++j) {
            if (board[i][j] == player.disc && board[i][j+1] == player.disc && board[i][j+2] == player.disc && board[i][j+3] == player.disc) {
                return true;
            }
        }
    }

    for (int i = 0; i <= rows - 4; ++i) {
        for (int j = 0; j <= columns - 4; ++j) {
            if (board[i][j] == player.disc && board[i+1][j+1] == player.disc && board[i+2][j+2] == player.disc && board[i+3][j+3] == player.disc) {
                return true;
            }
        }
    }

    for (int i = 0; i <= rows - 4; ++i) {
        for (int j = columns - 1; j >= 3; --j) {
            if (board[i][j] == player.disc && board[i+1][j-1] == player.disc && board[i+2][j-2] == player.disc && board[i+3][j-3] == player.disc) {
                return true;
            }
        }
    }

    return false;
}


void placingDiscMP(char** &board, int rows, int columns, playerInfo player, int slot) {
    //este metodo se encarga de poner la ficha en la columna escogida por el jugador, pero debe ser en la fila mas baja disponible (la ultima vacia de arriba hacia abajo)
    printf("%s 's move is %d \n",player.nickname,slot);
    int row = rows - 1;
    while (row >= 0 && board[row][slot] != '_') {
        --row;
    }
    //se sale del loop si encuentra un espacio vacio y despues verifica que dicho espacio sea valido (mayor a 0) antes de colocarla
    if (row >= 0) {
        board[row][slot] = player.disc;
    }
}

int phanMove(char** &board, int rows, int columns, playerInfo ai, playerInfo opponent) {
//Phan es el apodo que le inventamos a la ia
//la ia empieza simulando jugadas en todas las columnas posibles, y si encuentra que hay un gane ya sea de la ia misma o del oponente, retorna la columna en la que se deberia poner, para ganar o bloquear gane "enemigo"
    for (int j = 0; j < columns; ++j) {
        for (int i = rows - 1; i >= 0; --i) { //los for's invertidos ayudan a buscar desde abajo hacia arriba
            if (board[i][j] == '_') {
                board[i][j] = ai.disc;
                if (checkWin(board, rows, columns, ai)) {
                    board[i][j] = '_';
                    return j;
                }
                board[i][j] = '_'; 
                break; 
            }
        }
    }

    for (int j = 0; j < columns; ++j) {
        for (int i = rows - 1; i >= 0; --i) {
            if (board[i][j] == '_') {
                board[i][j] = opponent.disc;
                if (checkWin(board, rows, columns, opponent)) {
                    board[i][j] = '_'; 
                    return j;
                }
                board[i][j] = '_'; 
                break; 
            }
        }
    }

    //normalmente, si la ia no encuentra ganes de ninguno de los dos lados, pondría el disco en una posicion aleatoria, pero en vez de hacerlo así, 
    //implementamos un metodo que, igualmente, simula jugadas y calcula sus scores. la jugada con el score más alto será retornada para que la ia la juegue. 
  
   int* opScores = new int[columns]; //arreglo para almacenar los scores de las jugadas si se toma en cuenta las jugadas ya existentes del oponente
   int highestOpScore = 0;
   int highestOpScoreIndex = 0;


   for (int i = 0; i < columns; ++i) {
        opScores[i] = evaluatingBoard(board, opponent, rows, columns, i+1); //obtiene los scores de cada una de las jugadas
        if (opScores[i]>=highestOpScore) { //compara el score obtenido al actual más alto, para ver si hay que actualizarlo
            highestOpScore=opScores[i];
            highestOpScoreIndex = i; //si si hay que actualizarlo, se cambia tanto el score como el index 
            //(realmente se podria retornar de un splo el opScores[i] sin almacenarlo en una variable pero lo hicimos así para tener mayor control)
        }
   }
    
    


   int* AIScores = new int[columns]; //se hace lo mismo que antes pero con las jugadas de AI
   int highestAIScore = 0;
   int highestAIScoreIndex = 0;

   for (int i = 0; i < columns; ++i) {
        AIScores[i] = evaluatingBoard(board, ai, rows, columns, i+1);
        if (AIScores[i]>=highestAIScore) {
            highestAIScore=AIScores[i];
            highestAIScoreIndex = i;
        }
   }

    //crear un arreglo donde los elementos sean la combinacion de las jugadas tomando en cuenta discos existentes de la IA y del oponente, para que el escoja sea el verdadero mejor
    int* combinedScores = new int[columns];
    int bestCombinedScore = 0;
    int bestCombinedScoreIndex = 0;

    for (int i = 0; i < columns; ++i) {
        combinedScores[i] = opScores[i] + AIScores[i] ;
        if (combinedScores[i]>=bestCombinedScore) {
            bestCombinedScore=combinedScores[i];
            bestCombinedScoreIndex = i;
        }
   }

    delete[] opScores;
    delete[] AIScores;
    delete[] combinedScores;

    return bestCombinedScoreIndex;

}


void growBoard(char** &board, int &rows, int &cols) {
    //este metodo va a agrandar la matriz
    //todo lo impreso en esta seccion es para pruebas de consola
    
    char whereToAdd; //'r' para row arriba, 'l' para col left, 'c' para col right
    //dependiendo de donde escoja el usuario agrandar la matriz, se alteraran las siguientes variables:
    int newRows;
    int newLeftCols;
    int newRightCols;

    printf("Where do you want it to grow?");
    cin >> whereToAdd; //nota: cuando lo corríamos con scanf, se caía el programa, e investigamos sobre porque pasaba eso y la única forma en la que logramos solucionarlo fue usando cinn en vez de scanf
    //scanf("%c", &whereToAdd); [se nos cae con esto]

    if (whereToAdd == 'r') {

        newRows = rows + 1;

        char** newBoard = createBoard(newRows, cols); //aloca memoria con el metodo de create board, solo que con nuevas dimensiones
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                newBoard[i + 1][j] = board[i][j]; //copia el board de arriba a abajo (por eso el [i+1]. la fila de mas arriba quedara vacia
            }
        }

        for (int i = 0; i < rows; ++i) {
            free(board[i]);
        }
        free(board);  //liberar el espacio de la matriz

        board = newBoard; //guarda las nuevas dimensiones y la matriz nueva
        rows = newRows;

        printf("This is your new board:\n");
        printBoard(board, rows, cols);

    } else if (whereToAdd == 'c') {
         //logica similar para expansion derecha o izquierda

        newLeftCols = cols + 1;

        char** newBoard = createBoard(rows, newLeftCols);

       for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                newBoard[i][j] = board[i][j]; //la deja exactamente igual porque agrega por defaulta hacia la izquierda entonces no hay que correrla
            }
        }


        for (int j = 0; j < rows; ++j) {
            free(board[j]);
        }
        free(board);

        board = newBoard;
        cols = newLeftCols;

        printf("This is your new board:\n");
        printBoard(board, rows, cols);
        
    } else if (whereToAdd=='l') {
        newRightCols = cols + 1;

        char** newBoard = createBoard(rows, newRightCols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            newBoard[i][j + 1] = board[i][j];//al igual que con la forma en la que se crece para arriba, este se tiene que correr todo un espacio mas a la derecha para dejar la columna izquierda nueva vacia
        }
    }

        for (int j = 0; j < rows; ++j) {
            free(board[j]);
        }
        free(board);

        board = newBoard;
        cols = newRightCols;

        printf("This is your new board:\n");
        printBoard(board, rows, cols);
    }
}

int evaluatingBoard (char ** &board, playerInfo player,int rows, int cols, int chosenColumn) {
    int totalScore = 0;
    if (chosenColumn < 0 || chosenColumn >= cols || board[0][chosenColumn] != '_') {
        //maneja si la columna escogida se sale del scope de la matriz (por arriba o por debajo), o si esta llena
        return totalScore; //su score seria 0, es decir, el peor
    }
    
    int availableRow = findAvailableRow(board, rows, cols, chosenColumn); //se encuentra en que fila se acomodaria la ficha (la primera más vacía de abajo hacia arriba)
    board[availableRow][chosenColumn] = player.disc; //simula la jugada

        //calcula los scores llamando las funciones de abajo
        totalScore += evaluateVertical(board, player, availableRow, chosenColumn, rows, cols);
        totalScore += evaluateHorizontal(board, player, availableRow, chosenColumn, rows, cols);
        totalScore += evaluateDiagonal(board, player, availableRow, chosenColumn, rows, cols);
 
       
    board[availableRow][chosenColumn] = '_'; //retorna la matriz a como solía estar antes de simular la jugada

    return totalScore;
}


int findAvailableRow(char**&board, int rows, int columns, int chosenColumn) {
    //este metodo se encarga de poner la ficha en la columna escogida por el jugador, pero debe ser en la fila mas baja disponible (la ultima vacia de arriba hacia abajo)
    //es igual que el placingDisc solo que no nos sirve que 'ponga' la ficha en el tablero entonces queremos que nos tire nada mas en cual fila la pondria potencialmente
        int row = rows - 1;
        while (row >= 0 && board[row][chosenColumn] != '_') {
            --row;
        }
        //se sale del loop si encuentra un espacio vacio y despues verifica que dicho espacio sea valido (mayor a 0) antes de colocarla
        if (row >= 0) {
            return row;

        }
    return -1; //no encontró la fila
}

int evaluateVertical (char**& board, playerInfo player, int availableRow, int chosenColumn, int rows, int columns) {
    int lookingUp = availableRow + 1;
    int count = 0; //almacena cuantas fichas del mismo tipo vio hacia arriba


    while(lookingUp < rows && board[lookingUp][chosenColumn] == player.disc) { //si la encuentra, aumenta el count y sube una
        count++; 
        lookingUp++;
    }

    int totalScore = -100; //lo pone en el valor minimo posible (-100 es un poco exagerado pero es para que no lo confunda con jugadas no inicializadas)

    if (count>=4) {
        totalScore = 100; // lo setea en valor máximo
    } else {
        totalScore = count;
    }

    return totalScore;
}

int evaluateHorizontal (char**& board, playerInfo player, int availableRow, int chosenColumn, int rows, int columns){
    //misma logica solo que para izquierda y derecha
    int lookingLeft = chosenColumn-1;
    int lookingRight = chosenColumn+1;
    int count = 0;

    while (lookingLeft>=0 && board[availableRow][lookingLeft] == player.disc) {
        count++;
        lookingLeft--;
    }

    while(lookingRight< columns && board[availableRow][lookingRight] == player.disc) {
        count++;
        lookingRight++;
    }

    int totalScore = -100;

    if (count>=4) {
        totalScore = 100;
    } else {
        totalScore = count;
    }

    return totalScore;

}

int evaluateDiagonal(char**& board, playerInfo player, int availableRow, int chosenColumn, int rows, int columns) {
    int count = 0;
    int totalScore = -100;


    //revisar hacia arriba, izquierda
    int row = availableRow + 1;
    int col = chosenColumn - 1;
    while (row < rows && col >= 0 && board[row][col] == player.disc) {
        count++;
        row++;
        col--;
    }

    //revisar hacia abajo, derecha
    row = availableRow - 1;
    col = chosenColumn + 1;
    while (row >= 0 && col < columns && board[row][col] == player.disc) {
        count++;
        row--;
        col++;
    }

   
    if (count >= 4) {
        totalScore = 100;
    } else {
        totalScore = count;
    }

    
    if (totalScore != 100) { //si es la maxima puntuacion, entonces se puede retornar asi. Si no, hay posibilidades de que la otra diagonal si sea la maxima
        count = 0;

       //revisar hacia arriba, derecha
        row = availableRow + 1;
        col = chosenColumn + 1;
        while (row < rows && col < columns && board[row][col] == player.disc) {
            count++;
            row++;
            col++;
        }

        //revisar hacia abajo, izquierda
        row = availableRow - 1;
        col = chosenColumn - 1;
        while (row >= 0 && col >= 0 && board[row][col] == player.disc) {
            count++;
            row--;
            col--;
        }

        if (count >= 4) {
            totalScore = 100;
        } else {
            totalScore = count;
        }
    }

    return totalScore;
}