#include <QtGui>
#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include <ctime>
#include <gmpxx.h>
#include "encapp.hpp"

typedef std::pair<mpz_class, mpz_class> dhpair;

namespace
{

  std::vector<dhpair> const dh_data {
    { mpz_class("0xB10B8F96A080E01DDE92DE5EAE5D54EC52C99FBCFB06A3C69A6A9DCA52D23B616073E28675A23D189838EF1E2EE652C013ECB4AEA906112324975C3CD49B83BFACCBDD7D90C4BD7098488E9C219A73724EFFD6FAE5644738FAA31A4FF55BCCC0A151AF5F0DC8B4BD45BF37DF365C1A65E68CFDA76D4DA708DF1FB2BC2E4A4371"),
      mpz_class("0xA4D1CBD5C3FD34126765A442EFB99905F8104DD258AC507FD6406CFF14266D31266FEA1E5C41564B777E690F5504F213160217B4B01B886A5E91547F9E2749F4D7FBD7D3B9A92EE1909D0D2263F80A76A6A24C087A091F531DBF0A0169B6A28AD662A4D18E73AFA32D779D5918D08BC8858F4DCEF97C2A24855E6EEB22B3B2E5") },
      { 13, 19 }
  };

  mpz_class p, g, ga, gb, gab;
  mpz_class limit;
  gmp_randstate_t rngstate;

  inline void write_mpz(QLineEdit * le, mpz_class const & z)
  {
    std::ostringstream os;
    os << z;
    le->setText(QString::fromStdString(os.str()));

  }

  inline void set_to_random(QLineEdit * le)
  {
    mpz_class r;
    mpz_urandomb(r.get_mpz_t(), rngstate, 256);
    write_mpz(le, r);
  }

  std::string addPadding(std::string s, std::size_t len = 100)
  {
    if (s.size() + 2 > len) return std::string(len, '\0');
    s.resize(len, static_cast<char>(len - s.size()));
    return s;
  }

  std::string stripPadding(std::string const & s)
  {
    if (s.empty()) return "[Error]";

    std::size_t n = s.back();

    if (n == 0 || n > s.size()) return "[Error]";

    return s.substr(0, s.size() - n);
  }

  mpz_class doEncrypt(std::string const & s)
  {
    mpz_class res(0);
    mpz_class secret(gab);

    for (auto it = s.cbegin(), end = s.cend(); it != end; ++it)
    {
      res *= 0x100;
      res += (secret % 0x100) ^ static_cast<unsigned char>(*it);
      secret /= 0x100;
    }

    return res;
  }

  std::string doDecrypt(mpz_class n, std::size_t N = 100)
  {
    std::string res;
    mpz_class secret(gab);

    std::vector<unsigned char> xors(N);

    for (std::size_t i = 0; i != N; ++i)
    {
      xors[i] = mpz_class(n % 0x100).get_ui();
      n /= 0x100;
    }

    for (auto it = xors.crbegin(), end = xors.crend(); it != end; ++it)
    {
      res.push_back(static_cast<unsigned char>(mpz_class((secret % 0x100) ^ *it).get_ui()));
      secret /= 0x100;
    }
    return res;
  }
}

myEncApp::myEncApp(QWidget *)
{
  setupUi(this);

  gmp_randinit_default(rngstate);
  gmp_randseed_ui(rngstate, std::time(NULL));

  /* Make sure the shared secret is at least as big as limit */
  mpz_setbit(limit.get_mpz_t(), 101);

  p = dh_data[0].first;
  g = dh_data[0].second;

  write_mpz(primeLineEdit, p);
  write_mpz(generatorLineEdit, g);
}

void myEncApp::runAlice()
{
  g = generatorLineEdit->text().toStdString().c_str();
  p = primeLineEdit->text().toStdString().c_str();

  mpz_class a(aliceSSecretALineEdit->text().toStdString().c_str());
  mpz_powm(ga.get_mpz_t(), g.get_mpz_t(), a.get_mpz_t(), p.get_mpz_t());
  write_mpz(aGALineEdit, ga);
  write_mpz(outputLineEdit, ga);
}

void myEncApp::runBob()
{
  mpz_class b(bobSSecretBLineEdit->text().toStdString().c_str());
  mpz_powm(gb.get_mpz_t(), g.get_mpz_t(), b.get_mpz_t(), p.get_mpz_t());
  mpz_powm(gab.get_mpz_t(), ga.get_mpz_t(), b.get_mpz_t(), p.get_mpz_t());
  write_mpz(bGBLineEdit, gb);
  write_mpz(outputLineEdit, gb);
  write_mpz(sharedSecretLineEdit, gab);
}

void myEncApp::readAlice()
{
  gb = mpz_class(inputLineEdit->text().toStdString().c_str());
  mpz_class a(aliceSSecretALineEdit->text().toStdString().c_str());
  bGBLineEdit->setText(inputLineEdit->text());
  mpz_powm(gab.get_mpz_t(), gb.get_mpz_t(), a.get_mpz_t(), p.get_mpz_t());
  write_mpz(sharedSecretLineEdit, gab);
}

void myEncApp::readBob()
{
  ga = mpz_class(inputLineEdit->text().toStdString().c_str());
  aGALineEdit->setText(inputLineEdit->text());
}

void myEncApp::updateMD5()
{
  gab = mpz_class(sharedSecretLineEdit->text().toStdString().c_str());
  md5content->setText(QString(QCryptographicHash::hash(sharedSecretLineEdit->text().toUtf8(), QCryptographicHash::Md5).toHex()));
}

void myEncApp::randomA()
{
  set_to_random(aliceSSecretALineEdit);
}

void myEncApp::randomB()
{
  set_to_random(bobSSecretBLineEdit);
}

void myEncApp::encrypt()
{
  std::string const msg(inputLineEdit->text().toStdString());
  write_mpz(outputLineEdit, doEncrypt(addPadding(msg)));
}

void myEncApp::decrypt()
{
  mpz_class const msg(inputLineEdit->text().toStdString());
  outputLineEdit->setText(QString::fromStdString(stripPadding(doDecrypt(msg))));
}
