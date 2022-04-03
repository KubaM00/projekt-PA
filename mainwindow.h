#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_dodajUzytkownika_clicked();

    void on_usunUzytkownika_clicked();

    void on_dodajKsiazke_clicked();

    void on_usunKsiazke_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase database;
    void przygotujBazeDanych();
    bool indeksPoprawny(QString indeks);
    bool tylkoLiteryOrazSpacje(QString dane);
    bool tylkoCyfry(QString dane);
    bool uzytkownikIstnieje(QString indeks);
    void zapiszUzytkownika(QString imie, QString nazwisko, QString indeks);
    void resetujKontrolkiNowegoUzytkownika();
    void usunWypozyczeniaUzytkownika(QString indeks);
    void usunUzytkownika(QString indeks);
    void resetujKontrolkiUsuwaniaUzytkownika();
    void zapiszKsiazke(QString tytul, QString autor, QString kod);
    void resetujKontrolkiNowejKsiazki();
    void usunWypozyczeniaKsiazki(QString kod);
    void usunKsiazke(QString kod);
    void resetujKontrolkiUsuwaniaKsiazki();
    void aktualizujTabeleUzytkownikow();
    void aktualizujTabeleKsiazek();
};
#endif // MAINWINDOW_H
