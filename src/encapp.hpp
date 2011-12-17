#ifndef H_ENCAPP
#define H_ENCAPP

#include "ui_encapp.h"

class myEncApp : public QDialog, private Ui::encapp
{
  Q_OBJECT

public:
  myEncApp(QWidget * parent = 0);

public slots:
  void runAlice();
  void runBob();
  void readAlice();
  void readBob();
  void updateMD5();
  void randomA();
  void randomB();
  void encrypt();
  void decrypt();
};


#endif
