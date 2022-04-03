#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    przygotujBazeDanych();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::przygotujBazeDanych()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("../Biblioteka 29.03/database.db");

    if(database.open())
    {
        QSqlQuery query;

        query.prepare("CREATE TABLE IF NOT EXISTS UZYTKOWNICY(indeks INTEGER UNIQUE PRIMARY KEY, imie VARCHAR(30) NOT NULL, nazwisko VARCHAR(30) NOT NULL);");
        if(!query.exec())
        {
            qDebug() << query.lastError();
        }

        query.prepare("CREATE TABLE IF NOT EXISTS KSIAZKI(kod INTEGER UNIQUE PRIMARY KEY, tytul VARCHAR(60), autor VARCHAR(60));");
        if(!query.exec())
        {
            qDebug() << query.lastError();
        }

        query.prepare("CREATE TABLE IF NOT EXISTS WYPOZYCZENIA(id INTEGER UNIQUE PRIMARY KEY AUTOINCREMENT, kod_ksiazki INTEGER, indeks_uzytkownika INTEGER, data_wypozyczenia DATE DEFAULT (datetime('now','localtime')), FOREIGN KEY (kod_ksiazki) REFERENCES KSIAZKI(kod), FOREIGN KEY (indeks_uzytkownika) REFERENCES UZYTKOWNICY(indeks));");
        if(!query.exec())
        {
            qDebug() << query.lastError();
        }

        aktualizujTabeleUzytkownikow();
        aktualizujTabeleKsiazek();
    }
    else
    {
        qDebug() << "Nie udalo sie otworzyc bazy danych.";
    }
}

void MainWindow::on_dodajUzytkownika_clicked()
{

    QString imie = ui->nowyUzytkownikImie->text();
    QString nazwisko = ui->nowyUzytkownikNazwisko->text();
    QString indeks = ui->nowyUzytkownikIndeks->text();

    if(!(tekstPoprawny(imie) && tekstPoprawny(nazwisko) && indeksPoprawny(indeks)))
    {
        qDebug() << "Nieprawidłowe dane użytkownika!";
        return;
    }

    if(uzytkownikIstnieje(indeks))
    {
        qDebug() << "Użytkownik o podanym indeksie już jest zarejestrowany!";
        return;
    }

    zapiszUzytkownika(imie, nazwisko, indeks);
    resetujKontrolkiNowegoUzytkownika();
}

bool MainWindow::indeksPoprawny(QString indeks)
{
    return tylkoCyfry(indeks) && indeks.size() == 6;
}

bool MainWindow::tylkoLiteryOrazSpacje(QString dane)
{
    for(QChar znak : dane)
    {
        if(!znak.isLetter() && !znak.isSpace())
        {
            return false;
        }
    }
    return true;
}

bool MainWindow::tylkoCyfry(QString dane)
{
    for(QChar znak : dane)
    {
        if(!znak.isDigit())
        {
            return false;
        }
    }
    return true;
}

bool MainWindow::uzytkownikIstnieje(QString indeks)
{
    QSqlQuery query;
    if(query.exec("SELECT COUNT(indeks) FROM UZYTKOWNICY WHERE indeks = '" + indeks + "';"))
    {
        query.next();
        return query.value(0).toInt() == 1;
    }
    else
    {
        qDebug() << query.lastError().text();
    }
    return false;
}

void MainWindow::zapiszUzytkownika(QString imie, QString nazwisko, QString indeks)
{
    QSqlQuery query;

    query.prepare("INSERT INTO UZYTKOWNICY VALUES('" + indeks + "', '" + imie + "', '" + nazwisko +"')");
    if(!query.exec())
    {
        qDebug() << query.lastError();
    }

    aktualizujTabeleUzytkownikow();
}
void MainWindow::on_usunUzytkownika_clicked()
{
    QString indeks = ui->usunUzytkownikaIndeks->text();
    bool potwierdzenie = ui->usunUzytkownikaPotwierdzenie->isChecked();

    if(!uzytkownikIstnieje(indeks))
    {
        qDebug() << "Nie znaleziono uzytkownika o podanym indeksie!";
        return;
    }

    if(!potwierdzenie)
    {
        qDebug() << "Aby kontynuować konieczne jest potwierdzenie!";
        return;
    }

    usunWypozyczeniaUzytkownika(indeks);
    usunUzytkownika(indeks);
    resetujKontrolkiUsuwaniaUzytkownika();
}

void MainWindow::usunWypozyczeniaUzytkownika(QString indeks)
{
    QSqlQuery query;

    query.prepare("DELETE FROM WYPOZYCZENIA WHERE indeks_uzytkownika = '" + indeks + "';");
    if(!query.exec())
    {
        qDebug() << query.lastError();
    }
}

void MainWindow::usunUzytkownika(QString indeks)
{
    QSqlQuery query;

    query.prepare("DELETE FROM UZYTKOWNICY WHERE indeks = '" + indeks + "';");
    if(!query.exec())
    {
        qDebug() << query.lastError();
    }

    aktualizujTabeleUzytkownikow();
}

void MainWindow::on_dodajKsiazke_clicked()
{
    QString tytul = ui->nowaKsiazkaTytul->text();
    QString autor = ui->nowaKsiazkaAutor->text();
    QString kod = ui->nowaKsiazkaKod->text();

    if(!(tekstPoprawny(tytul) && tekstPoprawny(autor, 5) && kodPoprawny(kod)))
    {
        qDebug() << "Nieprawidłowe dane książki!";
        return;
    }

    if(ksiazkaIstnieje(kod))
    {
        qDebug() << "Książka o podanym kodzie już istnieje!";
        return;
    }

    zapiszKsiazke(tytul, autor, kod);
    resetujKontrolkiNowejKsiazki();
}

void MainWindow::zapiszKsiazke(QString tytul, QString autor, QString kod)
{
    QSqlQuery query;

    query.prepare("INSERT INTO KSIAZKI VALUES ('" + kod + "', '" + tytul + "', '" + autor + "');");
    if(!query.exec())
    {
        qDebug() << query.lastError();
    }

    aktualizujTabeleKsiazek();
}

void MainWindow::on_usunKsiazke_clicked()
{
    QString kod = ui->usunKsiazkeKod->text();
    bool potwierdzenie = ui->usunKsiazkePotwierdzenie->isChecked();

    if(!ksiazkaIstnieje(kod))
    {
        qDebug() << "Nie znaleziono ksiazki o podanym kodzie!";
        return;
    }

    if(!potwierdzenie)
    {
        qDebug() << "Aby kontynuować konieczne jest potwierdzenie!";
        return;
    }

    usunWypozyczeniaKsiazki(kod);
    usunKsiazke(kod);
    resetujKontrolkiUsuwaniaKsiazki();
}

void MainWindow::usunWypozyczeniaKsiazki(QString kod)
{
    QSqlQuery query;

    query.prepare("DELETE FROM WYPOZYCZENIA WHERE kod_ksiazki = '" + kod + "';");
    if(!query.exec())
    {
        qDebug() << query.lastError();
    }
}

void MainWindow::usunKsiazke(QString kod)
{
    QSqlQuery query;

    query.prepare("DELETE FROM KSIAZKI WHERE kod = '" + kod + "';");
    if(!query.exec())
    {
        qDebug() << query.lastError();
    }

    aktualizujTabeleKsiazek();
}
void MainWindow::aktualizujTabeleUzytkownikow()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT * FROM UZYTKOWNICY;");
    query.exec();
    model->setQuery(query);
    ui->tabelaUzytkownikow->setModel(model);
}

void MainWindow::aktualizujTabeleKsiazek()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT * FROM KSIAZKI;");
    query.exec();
    model->setQuery(query);
    ui->tabelaKsiazek->setModel(model);
}
