#include <QApplication>
#include <memory>
#include "encapp.hpp"

int main(int argc, char * argv[])
{
  QApplication app(argc, argv);
  std::unique_ptr<myEncApp> dialog(new myEncApp);
  dialog->show();
  return app.exec();
}
