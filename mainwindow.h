#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qpushbutton.h"
#include <QMainWindow>

//Este es el archivo de declaraciones de funciones, estructuras, clases, variables necesariaas para ejecutar el codigo
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void gameOver();

private slots:
    void onButtonClicked();

private:
    Ui::MainWindow *ui;

    static const int ROWS = 6;
    static const int COLS = 7;
    int winRow;
    int winCol;
    QPushButton *boardButtons[ROWS][COLS];
    char** board;
    bool multiplayerMode;
    bool currentPlayer;

    struct playerInfo {
        QString name;
        char disc;
    };
    void startGame();
    void startSinglePlayerGame();
    void startMultiplayerGame();
    char** createMatrix();
    void createBoard();
    void updateBoardUI();
    bool checkWin(int row, int col, char** boardToCheck);
    std::pair<int,int> phanMove(char** &board, int rows, int columns, playerInfo ai, playerInfo opponent);
    void dropPiece(int row, int col);
    void resetBoard();
};

#endif // MAINWINDOW_H
