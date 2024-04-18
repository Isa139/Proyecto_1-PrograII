#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QMessageBox>
#include <cstdlib>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QPropertyAnimation>
#include <QEasingCurve>

//Constructor donde se pone el titulo de la ventana principal y se crean los botones para poder seleccionar el modo de juego, ademas se conectan los botones a las funciones respectivas
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), multiplayerMode(false), currentPlayer(false)
{
    ui->setupUi(this);
    setWindowTitle("Connect Four");

    // Set up game mode selection
    QPushButton *singlePlayerButton = new QPushButton("Single Player", this);
    QPushButton *multiPlayerButton = new QPushButton("Multiplayer", this);

    connect(singlePlayerButton, &QPushButton::clicked, this, &MainWindow::startSinglePlayerGame);
    connect(multiPlayerButton, &QPushButton::clicked, this, &MainWindow::startMultiplayerGame);

    QVBoxLayout *modeLayout = new QVBoxLayout();
    modeLayout->addWidget(singlePlayerButton);
    modeLayout->addWidget(multiPlayerButton);
    QWidget *modeWidget = new QWidget(this);
    modeWidget->setLayout(modeLayout);

    setCentralWidget(modeWidget);
}
//Destructor para liberar memoria en ui
MainWindow::~MainWindow() {
    delete ui;
}
//Las siguientes dos funciones inician los respectivos modos de juego, multiplayer y single player con la funcion startGame
void MainWindow::startSinglePlayerGame() {
    multiplayerMode = false;
    startGame();
}

void MainWindow::startMultiplayerGame() {
    multiplayerMode = true;
    startGame();
}
//Inicia el juego inicializando la matriz
void MainWindow::startGame() {
    board = createMatrix();
    createBoard();
}

//Se inicializa la matriz
char** MainWindow::createMatrix(){
    // Memoria para rows
    char** result = new char*[ROWS];

    // Mmeoria para columnas
    for (int i = 0; i < ROWS; ++i) {
        result[i] = new char[COLS];
    }

    // Inicializa la amtri con sus elementos
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            result[i][j] = '_';
        }
    }
    return result;
}
//Crea el tablero de juego agregando botones que representan los slots de un juego de conecta 4 real y los agrega al layout (basicamente crea la ventana donde se juega)
void MainWindow::createBoard()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QGridLayout *layout = new QGridLayout(centralWidget);
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            QPushButton *button = new QPushButton(centralWidget);
            button->setFixedSize(50, 50);
            button->setText(" ");
            connect(button, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
            layout->addWidget(button, row, col);
            boardButtons[row][col] = button;
        }
    }
}
//Se encarga de manejar los clicks del mouse y de poner las fichas en el tablero, verifica si hay ganadores despues de los clicks y ademas implementa la logica de la AI en single player
void MainWindow::onButtonClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    int col = button->parentWidget()->layout()->indexOf(button) % COLS;
    int row = ROWS - 1;

    // Encuentra el slot mas abajo en la columna
    while (row >= 0 && board[row][col] != '_') {
        --row;
    }

    // Si esta vacio pone una ficha
    if (row >= 0) {
        // colores segun cada jugador
        if (currentPlayer)
            button->setStyleSheet("background-color: red;");
        else
            button->setStyleSheet("background-color: blue;");
        // Updates del board y de la ui
        board[row][col] = currentPlayer ? 'r' : 'b';

        updateBoardUI();

        // revisa ganes
        if (checkWin(row, col, board)) {
            QMessageBox::information(this, "Game Over", "Player " + QString(currentPlayer ? "2" : "1") + " wins!");
            emit gameOver();
            resetBoard();
            return;
        }

        // cambio al siguiente jugador
        currentPlayer = !currentPlayer;

        // se encarga del modo single player
        if (!multiplayerMode && currentPlayer) {
            playerInfo ai = {"AI", 'b'}; // AI es azul
            playerInfo player = {"Player", 'r'}; // Humano es rojo

            std::pair<int, int> aiMove = phanMove(board, ROWS - 1, COLS - 1, ai, player);
            if (aiMove.first != -1 && aiMove.second != -1) {
                int aiRow = ROWS - 1;
                while (aiRow >= 0 && board[aiRow][aiMove.second] != '_') {
                    --aiRow;
                }
                if (aiRow >= 0) {
                    QPushButton *aiButton = boardButtons[aiRow][aiMove.second];
                    aiButton->click();
                }
            }
        }
    }
}
//se encarga de como se ve el tablero en ui, pone los colores segun como avance el juego
void MainWindow::updateBoardUI()
{
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            QPushButton *button = boardButtons[row][col];
            if (board[row][col] == 'r')
                button->setStyleSheet("background-color: red;");
            else if (board[row][col] == 'b')
                button->setStyleSheet("background-color: blue;");
            else
                button->setStyleSheet("background-color: white;");
        }
    }
}
//se encarga de las animaciones de las fichas, como caen etc
void MainWindow::dropPiece(int row, int col) {
    QPushButton* button = boardButtons[row][col];
    int startY = button->y() - button->height();
    int endY = button->y();

    // Esto crea la animacion
    QPropertyAnimation* animation = new QPropertyAnimation(button, "geometry");
    animation->setDuration(1000);
    animation->setStartValue(QRect(button->x(), startY, button->width(), button->height()));
    animation->setEndValue(QRect(button->x(), endY, button->width(), button->height()));
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    // Esto conecta las animaciones
    connect(animation, &QPropertyAnimation::finished, this, [=](){
        delete animation;
    });
    animation->start();
}

//Esta funcion se encarga de verficar los ganes, vertical, horizontal y diagonales
bool MainWindow::checkWin(int row, int col, char** boardToCheck)
{
    char disc = boardToCheck[row][col];

    // horizontal
    for (int i = 0; i <= COLS - 4; ++i) {
        if (boardToCheck[row][i] == disc && boardToCheck[row][i+1] == disc && boardToCheck[row][i+2] == disc && boardToCheck[row][i+3] == disc) {
            return true;
        }
    }

    // vertical
    for (int i = 0; i <= ROWS - 4; ++i) {
        if (boardToCheck[i][col] == disc && boardToCheck[i+1][col] == disc && boardToCheck[i+2][col] == disc && boardToCheck[i+3][col] == disc) {
            return true;
        }
    }

    // diagonales
    for (int i = 0; i <= ROWS - 4; ++i) {
        for (int j = 0; j <= COLS - 4; ++j) {
            if (boardToCheck[i][j] == disc && boardToCheck[i+1][j+1] == disc && boardToCheck[i+2][j+2] == disc && boardToCheck[i+3][j+3] == disc) {
                return true;
            }
        }
    }
    for (int i = 0; i <= ROWS - 4; ++i) {
        for (int j = COLS - 1; j >= 3; --j) {
            if (boardToCheck[i][j] == disc && boardToCheck[i+1][j-1] == disc && boardToCheck[i+2][j-2] == disc && boardToCheck[i+3][j-3] == disc) {
                return true;
            }
        }
    }

    return false;
}
//esta funcion es la logica de la IA, se implementa en el modo single player (disclaimer: es bastante tonta, y por alguna razon hay darle doble click para que funcione, (pero en el backend funciona bien))
std::pair<int,int> MainWindow::phanMove(char** &board, int rows, int columns, playerInfo ai, playerInfo opponent)
{
    std::pair<int,int> bestMove;
    for (int j = 0; j < columns; ++j) {
        char** testBoard = createMatrix();
        for (int i = 0; i < rows; ++i) {
            for (int k = 0; k < columns; ++k) {
                testBoard[i][k] = board[i][k];
            }
        }
        for (int i = rows - 1; i >= 0; --i) {
            if (testBoard[i][j] == '_') {
                testBoard[i][j] = ai.disc;
                if (checkWin(i, j, testBoard)) {
                    bestMove.first = i;
                    bestMove.second = j;
                    return bestMove;
                }
                break;
            }
        }
    }
    for (int j = 0; j < columns; ++j) {
        char** testBoard = createMatrix();
        for (int i = 0; i < rows; ++i) {
            for (int k = 0; k < columns; ++k) {
                testBoard[i][k] = board[i][k];
            }
        }
        for (int i = rows - 1; i >= 0; --i) {
            if (testBoard[i][j] == '_') {
                testBoard[i][j] = opponent.disc;
                if (checkWin(i, j, testBoard)) {
                    bestMove.first = i;
                    bestMove.second = j;
                    return bestMove;
                }
                break;
            }
        }
    }

    int randomRow = rand()%(ROWS-1 + 1) + 1;
    int randomCol = rand()%(COLS-1 + 1) + 1;
    std::pair<int, int> random = std::make_pair(randomRow,randomCol);
    return random;
}
//se encarga de reiniciar el tablero de juego cuando alguien ya gana para poder jugar de nuevo
void MainWindow::resetBoard() {
    // Reset the board state and UI
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            board[row][col] = '_';
            boardButtons[row][col]->setText(" ");
            boardButtons[row][col]->setStyleSheet(""); // Reset button style
        }
    }
}
