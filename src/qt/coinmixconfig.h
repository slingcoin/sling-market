#ifndef COINMIXCONFIG_H
#define COINMIXCONFIG_H

#include <QDialog>

namespace Ui
{
class CoinMixConfig;
}
class WalletModel;

/** Multifunctional dialog to ask for passphrases. Used for encryption, unlocking, and changing the passphrase.
 */
class CoinMixConfig : public QDialog
{
    Q_OBJECT

public:
    CoinMixConfig(QWidget* parent = 0);
    ~CoinMixConfig();

    void setModel(WalletModel* model);


private:
    Ui::CoinMixConfig* ui;
    WalletModel* model;
    void configure(bool enabled, int coins, int rounds);

private slots:

    void clickBasic();
    void clickHigh();
    void clickMax();
};

#endif // COINMIXCONFIG_H
